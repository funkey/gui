
#include <util/Logger.h>
#include "ContextSettings.h"
#include "GlContext.h"
#include "GlContextCreator.h"
#include "OpenGl.h"

logger::LogChannel opengllog("opengllog", "[OpenGl] ");

namespace gui {

namespace priv {

void glCheckError(const char* file, const char* function, unsigned int line) {

	GLenum error = glGetError();

	if (error != GL_NO_ERROR)
		BOOST_THROW_EXCEPTION(
				boost::enable_error_info(OpenGlError())
						<< boost::throw_function(function)
						<< boost::throw_file(file)
						<< boost::throw_line(line)
						<< error_message(static_cast<const char*>((void*)gluErrorString(error)))
						<< STACK_TRACE);
}

} // namespace priv

OpenGl* TheOpenGl = 0;

OpenGl::OpenGl() :
	_globalContext(0) {

	LOG_DEBUG(opengllog) << "creating global glxContext" << std::endl;

	// use default context settings
	ContextSettings defaultSettings;

	// create offline context (has a dummy window)
	_globalContext = new GlContext(defaultSettings);

	// set active for this thread
	if (!_globalContext->activate())
		LOG_ERROR(opengllog) << "failed to activate global context" << std::endl;

	// initialize GLEW for extension checks
	if (glewInit() != GLEW_OK)
		LOG_ERROR(opengllog) << "GLEW did not initialize" << std::endl;

	// see if pixel buffer objects are supported
	if (!glewIsSupported("GL_ARB_pixel_buffer_object"))
		LOG_ERROR(opengllog) << "pixel buffer extension not supported" << std::endl;

	LOG_ALL(opengllog) << "Initialized" << std::endl;
}

OpenGl::~OpenGl() {

	LOG_ALL(opengllog) << "destructing..." << std::endl;

	_globalContext->activate(false);
	if (_globalContext)
		delete _globalContext;

	LOG_ALL(opengllog) << "destructed" << std::endl;
}

OpenGl*
OpenGl::getInstance() {

	if (TheOpenGl == 0)
		TheOpenGl = new OpenGl();

	return TheOpenGl;
}

boost::mutex&
OpenGl::getMutex() {

	return getInstance()->_mutex;
}

GlContext*
OpenGl::getGlobalContext() {

	return getInstance()->_globalContext;
}

void
OpenGl::flush() {

	LOG_ALL(opengllog) << "attempting to flush current context" << std::endl;

	GlContext* context = getInstance()->_context.get();

	if (context != 0)
		context->flush();
	else
		LOG_ALL(opengllog) << "there is no current context in this thread" << std::endl;
}

OpenGl::Guard::Guard() :
	_openGl(getInstance()) {

	LOG_ALL(opengllog) << "[Guard] creating new guard" << std::endl;

	// if no context was set for this thread
	if (_openGl->_context.get() == 0) {

		LOG_ALL(opengllog) << "[Guard] creating new context for this thread" << std::endl;

		// create a new offline context
		ContextSettings defaultSettings;
		GlContext* context = new GlContext(defaultSettings, OpenGl::getGlobalContext());

		// set it
		_openGl->_context.reset(context);
	}

	// if current context is active already
	if (_openGl->_context->isActive()) {

		LOG_ALL(opengllog) << "[Guard] current context is active already -- nothing to do here" << std::endl;

		_deactivateContext = false;

	} else {

		LOG_ALL(opengllog) << "[Guard] activating context" << std::endl;

		// activate the context for this thread
		if (!_openGl->_context->activate())
			LOG_ERROR(opengllog) << "[Guard] failed to activate context for this thread" << std::endl;

		// remember to deactivate it on destruction
		_deactivateContext = true;
	}

	// reset OpenGl settings
	glColor3f(1.0, 1.0, 1.0);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_TEXTURE_2D);
}

OpenGl::Guard::Guard(GlContextCreator* contextCreator) :
	_openGl(getInstance()) {

	LOG_ALL(opengllog) << "[Guard] creating new factory guard" << std::endl;

	if (contextCreator == 0) {

		LOG_ALL(opengllog) << "[Guard] destructing current thread's context" << std::endl;

		invalidateCurrentContext();

		_deactivateContext = false;

		return;
	}

	LOG_ALL(opengllog) << "[Guard] ensuring valid context" << std::endl;

	// remember to deactivate the context on destruction
	_deactivateContext = true;

	if (getPreviousContextCreator() == contextCreator && reusePreviousContext()) {

		LOG_ALL(opengllog) << "[Guard] could reuse previous context from the same creator" << std::endl;

		return;

	} else {

		LOG_ALL(opengllog) << "[Guard] previous context not present or invalid -- create a new one" << std::endl;

		createNewContex(contextCreator);
	}
}

OpenGl::Guard::~Guard() {

	LOG_ALL(opengllog) << "[Guard] destructing" << std::endl;

	if (_deactivateContext) {

		LOG_ALL(opengllog) << "[Guard] deactivating context" << std::endl;

		// deactivate the context for this thread
		_openGl->_context->activate(false);
	}
}

GlContextCreator*
OpenGl::Guard::getPreviousContextCreator() {

	if (_openGl->_contextCreator.get() != 0)

		return (*_openGl->_contextCreator.get());

	else

		return 0;
}

bool
OpenGl::Guard::createNewContex(GlContextCreator* contextCreator) {

	// set the current context to the one given by contextCreator
	_openGl->_context.reset(contextCreator->createGlContext());

	// remember who created this context
	_openGl->_contextCreator.reset(new GlContextCreator*(contextCreator));

	// activate the new context
	if (!_openGl->_context->activate()) {

		LOG_ERROR(opengllog) << "[Guard] failed to activate newly created context" << std::endl;

		return false;
	}

	return true;
}

bool
OpenGl::Guard::reusePreviousContext() {

	// is there a previous context and creator?
	if (_openGl->_context.get() == 0 || _openGl->_contextCreator.get() == 0)
		return false;

	// activate the previous context for this thread and check whether it is
	// still valid
	if (_openGl->_context->activate()) {

		LOG_ALL(opengllog) << "[Guard] previous context still working" << std::endl;

		// the context was valid, everything is fine
		return true;
	}

	// the previous context got invalid
	return false;
}

void
OpenGl::Guard::invalidateCurrentContext() {

	_openGl->_context.reset();

	_openGl->_contextCreator.reset();
}

} // namespace gui
