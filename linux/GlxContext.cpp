/* Parts of the source code of this file have been taken from the great SFML
 * project: http://www.sfml-dev.org/.
 */

#include <util/Logger.h>

#include <gui/linux/GlxContext.h>
#include <gui/Window.h>

using namespace logger;

LogChannel glxlog("glxlog", "[GlContext] ");

namespace gui {

GlContext::GlContext(ContextSettings& settings, GlContext* share) :
	_ownWindow(true),
	_glxContext(0),
	_active(false) {

	WindowMode mode;
	mode.mapped = false;

	_window = new Window("dummy window", mode);

	// initialize the context
	createContext(settings, share);
}

GlContext::GlContext(Window* window, ContextSettings& settings, GlContext* share) :
	_window(window),
	_ownWindow(false),
	_glxContext(0) {

	// initialize the context
	if (_window)
		createContext(settings, share);
}

GlContext::~GlContext() {

	// destroy glx window

	if (_glxWindow)
		glXDestroyWindow(_window->getDisplay(), _glxWindow);

	LOG_DEBUG(glxlog) << "destructing GlContext " << this << std::endl;

	// destroy the context

	if (_glxContext) {

		if (glXGetCurrentContext() == _glxContext)
			glXMakeCurrent(_window->getDisplay(), None, NULL);

		glXDestroyContext(_window->getDisplay(), _glxContext);
	}

	// destroy the window if we own it
	if (_window && _ownWindow) {

		delete _window;
		_window = 0;
	}
}

bool
GlContext::activate(bool active) {

	if (active) {

		if (!_glxContext) {

			LOG_ERROR(glxlog) << "attempted to activate an "
			                  << "uninitialised context" << std::endl;

			_active = false;

			return false;
		}

		if (!glXMakeContextCurrent(_window->getDisplay(), _glxDrawable, _glxDrawable, _glxContext)) {

			LOG_ERROR(glxlog) << "failed to make context current"
			                  << std::endl;

			_active = false;

			return false;
		}

		_active = true;

		return true;

	} else {

		_active = false;

		return glXMakeCurrent(_window->getDisplay(), None, NULL);
	}
}

bool
GlContext::isActive() {

	return _active;
}

void
GlContext::flush() {

	if (_window)
		glXSwapBuffers(_window->getDisplay(), _glxDrawable);
}

void
GlContext::enableVerticalSync(bool enable) {

	const GLubyte* name =
			reinterpret_cast<const GLubyte*>("glXSwapIntervalSGI");

	PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI =
			reinterpret_cast<PFNGLXSWAPINTERVALSGIPROC>(glXGetProcAddress(name));

	if (glXSwapIntervalSGI)
		glXSwapIntervalSGI(enable ? 1 : 0);
}

void
GlContext::createContext(ContextSettings& settings, GlContext* share) {

	_settings = settings;

	// create OpenGl context

	if (share)
		_glxContext = glXCreateNewContext(_window->getDisplay(), *_window->getFbConfig(), GLX_RGBA_TYPE, share->_glxContext, True);
	else
		_glxContext = glXCreateNewContext(_window->getDisplay(), *_window->getFbConfig(), GLX_RGBA_TYPE, 0, True);

	if (!_glxContext)
		BOOST_THROW_EXCEPTION(GuiError() << error_message("[GlContext] unable to create glx context") << STACK_TRACE);

	// create glx window

	_glxWindow = glXCreateWindow(_window->getDisplay(), *_window->getFbConfig(), *_window->getXcbWindow(), 0);

	if (!_glxWindow) {

		glXDestroyContext(_window->getDisplay(), _glxContext);

		BOOST_THROW_EXCEPTION(GuiError() << error_message("[GlContext] unable to create glx window") << STACK_TRACE);
	}

	_glxDrawable = _glxWindow;
}

} // namespace gui
