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
	_window(0),
	_ownWindow(true),
	_context(0),
	_active(false) {

	_display = XOpenDisplay(0);

	// create a dummy window to associate this context with
	int screen = DefaultScreen(_display);
	_window = XCreateWindow(
			_display,
			RootWindow(_display, screen),
			0, 0, 1, 1,
			0,
			DefaultDepth(_display, screen),
			InputOutput,
			DefaultVisual(_display, screen),
			0, NULL);

	// initialize the context
	createContext(settings, share);
}

GlContext::GlContext(Window* window, ContextSettings& settings, GlContext* share) :
	_window(0),
	_ownWindow(false),
	_context(0) {

	_display = window->getDisplay();
	_window  = window->getX11Window();

	// initialize the context
	if (_window)
		createContext(settings, share);
}

GlContext::~GlContext() {

	LOG_DEBUG(glxlog) << "destructing GlContext " << this << std::endl;

	// destroy the context
	if (_context) {

		if (glXGetCurrentContext() == _context)
			glXMakeCurrent(_display, None, NULL);

		glXDestroyContext(_display, _context);
	}

	// destroy the window if we own it
	if (_window && _ownWindow) {

		XDestroyWindow(_display, _window);
		XFlush(_display);
	}
	
	// close the connection with the X server
	if (_ownWindow)
		XCloseDisplay(_display);
}

bool
GlContext::activate(bool active) {

	if (active) {

		if (!_context) {

			LOG_ERROR(glxlog) << "attempted to activate an "
			                  << "uninitialised context" << std::endl;

			_active = false;

			return false;
		}

		if (!glXMakeCurrent(_display, _window, _context)) {

			LOG_ERROR(glxlog) << "failed to make context current"
			                  << std::endl;

			_active = false;

			return false;
		}

		_active = true;

		return true;

	} else {

		_active = false;

		return glXMakeCurrent(_display, None, NULL);
	}
}

bool
GlContext::isActive() {

	return _active;
}

void
GlContext::flush() {

	if (_window)
		glXSwapBuffers(_display, _window);
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

	// get the attributes of the target window
	XWindowAttributes windowAttributes;
	if (!XGetWindowAttributes(_display, _window, &windowAttributes)) {

		LOG_ERROR(glxlog) << "could not get window attributes" << std::endl;
		return;
	}

	// setup the visual infos to match
	XVisualInfo tpl;
	tpl.depth    = windowAttributes.depth;
	tpl.visualid = XVisualIDFromVisual(windowAttributes.visual);
	tpl.screen   = DefaultScreen(_display);

	// get all the visuals matching the template
	int numVisuals = 0;
	XVisualInfo* visuals =
			XGetVisualInfo(
					_display,
					VisualDepthMask |
					VisualIDMask |
					VisualScreenMask,
					&tpl,
					&numVisuals);

	if (!visuals || (numVisuals == 0)) {
		if (visuals)
			XFree(visuals);
		LOG_ERROR(glxlog) << "there is no valid visual "
		                  << "for the selected screen" << std::endl;
		return;
	}

	// find the best visual
	int bestScore  = -1;
	XVisualInfo* bestVisual = NULL;

	for (int i = 0; i < numVisuals; i++) {

		// get the current visual attributes
		int RGBA, doubleBuffer,
			red, green, blue,
			alpha, depth, stencil,
			multiSampling, samples;

		glXGetConfig(_display, &visuals[i], GLX_RGBA,               &RGBA);
		glXGetConfig(_display, &visuals[i], GLX_DOUBLEBUFFER,       &doubleBuffer);
		glXGetConfig(_display, &visuals[i], GLX_RED_SIZE,           &red);
		glXGetConfig(_display, &visuals[i], GLX_GREEN_SIZE,         &green);
		glXGetConfig(_display, &visuals[i], GLX_BLUE_SIZE,          &blue);
		glXGetConfig(_display, &visuals[i], GLX_ALPHA_SIZE,         &alpha);
		glXGetConfig(_display, &visuals[i], GLX_DEPTH_SIZE,         &depth);
		glXGetConfig(_display, &visuals[i], GLX_STENCIL_SIZE,       &stencil);
		glXGetConfig(_display, &visuals[i], GLX_SAMPLE_BUFFERS_ARB, &multiSampling);
		glXGetConfig(_display, &visuals[i], GLX_SAMPLES_ARB,        &samples);

		// check the mandatory parameters
		if ((RGBA == 0) || (doubleBuffer == 0))
			continue;

		// evaluate the current configuration
		int color = red + green + blue + alpha;
		int score = evaluateFormat(
				_settings,
				color,
				stencil,
				multiSampling ? samples : 0);

		// Keep it if it's better than the current best
		if (score < bestScore || bestScore == -1) {

			bestScore  = score;
			bestVisual = &visuals[i];
		}
	}

	// make sure that we have found a visual
	if (!bestVisual) {

		LOG_ERROR(glxlog) << "failed to find a suitable pixel format "
		              << "for the window -- cannot create OpenGL context"
		              << std::endl;
		return;
	}

	// get the context to share display lists with
	GLXContext toShare = share ? share->_context : 0;
	if (toShare)
		LOG_DEBUG(glxlog) << "sharing context" << std::endl;

	// create the OpenGL context -- first try context versions >= 3.0
	// if it is requested (they require special code)
	while (!_context && (_settings.majorVersion >= 3)) {

		const GLubyte* name =
				reinterpret_cast<const GLubyte*>("glXCreateContextAttribsARB");

		PFNGLXCREATECONTEXTATTRIBSARBPROC glXCreateContextAttribsARB =
				reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(
						glXGetProcAddress(name));

		if (glXCreateContextAttribsARB) {

			int numConfigs = 0;
			GLXFBConfig* configs =
					glXChooseFBConfig(
							_display,
							DefaultScreen(_display),
							NULL,
							&numConfigs);

			if (configs && numConfigs > 0) {

				// create the context
				int attributes[] = {

					GLX_CONTEXT_MAJOR_VERSION_ARB, _settings.majorVersion,
					GLX_CONTEXT_MINOR_VERSION_ARB, _settings.minorVersion,
					GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
					0, 0
				};

				_context =
						glXCreateContextAttribsARB(
								_display,
								configs[0],
								toShare,
								true,
								attributes);

				LOG_ALL(glxlog) << "created context for version "
				                << _settings.majorVersion << "."
				                << _settings.minorVersion << std::endl;
			}

			if (configs)
				XFree(configs);
		}

		// If we couldn't create the context, lower the version number and try again -- stop at 3.0
		// Invalid version numbers will be generated by this algorithm (like 3.9), but we really don't care
		if (!_context) {

			if (_settings.minorVersion > 0) {

				// If the minor version is not 0, we decrease it and try again
				_settings.minorVersion--;

			} else {

				// If the minor version is 0, we decrease the major version
				_settings.majorVersion--;
				_settings.minorVersion = 9;
			}
		}
	}

	// If the OpenGL >= 3.0 context failed or if we don't want one,
	// create a regular OpenGL 1.x/2.x context
	if (!_context) {

		LOG_ALL(glxlog) << "could not (or didn't want to) "
		                << "create 3.x context -- creating 2.0 instead" << std::endl;

		// set the context version to 2.0 (arbitrary)
		_settings.majorVersion = 2;
		_settings.minorVersion = 0;

		_context = glXCreateContext(_display, bestVisual, toShare, true);

		if (!_context) {

			LOG_ERROR(glxlog) << "failed to create an OpenGL context"
			                  << std::endl;
			return;
		}
	}

	// update the creation settings from the chosen format
	int depth, stencil, multiSampling, samples;
	glXGetConfig(_display, bestVisual, GLX_DEPTH_SIZE,         &depth);
	glXGetConfig(_display, bestVisual, GLX_STENCIL_SIZE,       &stencil);
	glXGetConfig(_display, bestVisual, GLX_SAMPLE_BUFFERS_ARB, &multiSampling);
	glXGetConfig(_display, bestVisual, GLX_SAMPLES_ARB,        &samples);
	_settings.depthBits         = static_cast<unsigned int>(depth);
	_settings.stencilBits       = static_cast<unsigned int>(stencil);
	_settings.antialiasingLevel = multiSampling ? samples : 0;

	// change the target window's colormap so that it matches the context's one
	::Window root =
			RootWindow(_display, DefaultScreen(_display));

	Colormap colorMap =
			XCreateColormap(
					_display,
					root,
					bestVisual->visual,
					AllocNone);

	XSetWindowColormap(_display, _window, colorMap);

	// Free the temporary visuals array
	XFree(visuals);
}

} // namespace gui
