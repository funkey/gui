#ifndef OPEN_GL_H__
#define OPEN_GL_H__

#include <string>

#include <boost/thread.hpp>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include <exceptions.h>

namespace gui {

namespace priv {

/**
 * Check for an OpenGl error. Throws an exception if an error was found.
 */
void glCheckError(const char* file, const char* function, unsigned int line);

#ifndef NDEBUG

	// executes an OpenGl call and throw an exception if an OpenGl error occured
	#define glCheck(call) ((call), gui::priv::glCheckError(__FILE__, BOOST_CURRENT_FUNCTION, __LINE__))

#else

	#define glCheck(call) (call)

#endif

} // namespace priv

// forward declaration
class GlContext;
class GlContextCreator;

// exceptions
struct OpenGlError : virtual GuiError {};

class OpenGl {

public:

	/**
	 * Instantiation of this class guarantees the calling thread
	 * to have a valid OpenGl context.
	 *
	 * This is guaranteed for the whole scope of the instantiation,
	 * that means in particular, it is exception safe.
	 */
	class Guard {

		public:

			/**
			 * Default contructor.
			 *
			 * Creates a new GlContext for the calling thread if none has been
			 * set so far and activates it. As soon as the thread stops, this
			 * context will be destructed.
			 */
			Guard();

			/**
			 * Replaces the current GlContext (if existing) for the current
			 * thread with the one created by contextCreator. If contextCreator
			 * is 0, this guard ensures that the previous context of this thread
			 * will be destructed.
			 *
			 * @param A pointer to a GlContextCreator that returns a new
			 *        GlContext or 0 to destruct the previous GlContext.
			 */
			Guard(GlContextCreator* contextCreator);

			/**
			 * Deactivates the current context.
			 */
			~Guard();

		private:

			GlContextCreator* getPreviousContextCreator();

			bool createNewContex(GlContextCreator* contextCreator);

			bool reusePreviousContext();

			void invalidateCurrentContext();

			// the singleton object
			OpenGl* _openGl;

			// indicates whether to deactivate the current context on destruction
			bool _deactivateContext;
	};

	/**
	 * Get the global opengl mutex.
	 *
	 * Protect every opengl operation to the global context with this mutex.
	 */
	static boost::mutex& getMutex();

	/**
	 * Get access to the global OpenGL context.
	 */
	static GlContext* getGlobalContext();

	/**
	 * Flush the currently active GlContext.
	 */
	static void flush();

private:

	/**
	 * This class is not supposed to be instantiated.
	 */
	OpenGl();

	/**
	 * Even singletons should be cleand up.
	 */
	~OpenGl();

	/**
	 * Get the singleton OpenGl object.
	 */
	static OpenGl* getInstance();

	boost::mutex _mutex;

	GlContext*   _globalContext;

	// a GlContext for the current context
	boost::thread_specific_ptr<GlContext> _context;

	// the factory that created the current context of the current thread
	boost::thread_specific_ptr<GlContextCreator*> _contextCreator;
};

} // namespace gui

#endif // OPEN_GL_H__

