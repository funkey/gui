#ifndef GLX_CONTEXT_H__
#define GLX_CONTEXT_H__

#include <X11/Xlib.h>

#include <gui/GlContextBase.h>
#include <gui/OpenGl.h>

namespace gui {

// forward declaration
class Window;

class GlContext : public GlContextBase {

public:

	/**
	 * Create an OpenGL context that is not attached to any window.
	 *
	 * @param share A GlContext to share display lists with.
	 */
	GlContext(ContextSettings& settings, GlContext* share = 0);

	/**
	 * Create an OpenGL context that renders into the specified window.
	 *
	 * @param window The owner of this context.
	 * @param settings The desired settings of the context.
	 * @param share A GlContext to share display lists with.
	 */
	GlContext(Window* window, ContextSettings& settings, GlContext* shared = 0);

	/**
	 * Release this context.
	 */
	virtual ~GlContext();

	/**
	 * Make this context the active context of the calling thread. If called
	 * with <code>false</code> as argument, deactivates this context (if it was
	 * active) and makes the offline context active for the calling thread.
	 */
	bool activate(bool active = true);

	/**
	 * Report whether this context is currently active.
	 *
	 * @return True, if this context is active.
	 */
	bool isActive();

	/**
	 * Make visible whatever was rendered using this context.
	 */
	void flush();

private:

	/**
	 * Create a context for the current window and display.
	 */
	void createContext(ContextSettings& settings, GlContext* share);

	/**
	 * Enables vertical sync if desired by ContextSettings.
	 */
	void enableVerticalSync(bool enable);

	// the window we are attached to (could have been created by us)
	Window* _window;

	// did we create the window?
	bool _ownWindow;

	// the glx window
	GLXWindow _glxWindow;

	// the glx drawable (which is the window)
	GLXDrawable _glxDrawable;

	// the true OpenGl context
	GLXContext _glxContext;

	// the settings according to this context
	ContextSettings _settings;

	// indicates that this context is currently active
	bool _active;
};

} // namespace gui

#endif // GLX_CONTEXT_H__

