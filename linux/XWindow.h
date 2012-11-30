#ifndef X_WINDOW_H__
#define X_WINDOW_H__

#include <string>

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <GL/glx.h>

#include <gui/WindowBase.h>
#include <gui/WindowMode.h>

using std::string;

namespace gui {

/**
 * Linux dependent implementation of the abstract class WindowBase.
 */
class XWindow : public WindowBase {

public:

	/**
	 * Default constructor.
	 *
	 * @param caption The caption of the window to be created.
	 * @param mode The requested mode of the window (size, position, ...)
	 */
	XWindow(string caption, const WindowMode& mode);

	/**
	 * Desctructor. Closes the window, if still open.
	 */
	~XWindow();

	/**
	 * Close this window.
	 */
	void close();

	/**
	 * Check whether the window was closed by the user.
	 *
	 * @return <code>true</code> if the window was closed.
	 */
	bool closed();

	/**
	 * Process all accumulated X events since the last call to this function.
	 * This method should be called repeatedly to ensure proper redrawing and
	 * user interaction.
	 */
	void processEvents();

	/**
	 * Process access to the X11 display to create and manage an OpenGl context.
	 */
	Display* getDisplay() { return _display; }

	/**
	 * Provides access to the xcb window to create an OpenGl context.
	 */
	xcb_window_t* getXcbWindow() { return &_xcbWindow; }

	/**
	 * Provides access to the frame-buffer configuration of this window to 
	 * create an OpenGl context.
	 */
	GLXFBConfig* getFbConfig() { return &_fbConfig; }

private:

	/**
	 * Converts an X keycode to a Key.
	 */
	keys::Key keycodeToKey(unsigned int keycode);

	/**
	 * Converts an X state to Modifiers.
	 */
	Modifiers stateToModifiers(unsigned int state);

	/**
	 * Converts an X button to a Button.
	 */
	buttons::Button buttonToButton(const xcb_button_t& xbutton);

	// the X11 display
	Display* _display;

	// the X11 screen
	int _screen;

	// the xcb connection
	xcb_connection_t* _xcbConnection;

	// the xcb screen
	xcb_screen_t* _xcbScreen;

	// the glx frame-buffer configuration for this window
	GLXFBConfig _fbConfig;

	// the xcb window
	xcb_window_t _xcbWindow;

	// atom for client delete events
	xcb_intern_atom_reply_t* _deleteReply;

	// is visible
	bool _visible;

	// was closed
	bool _closed;
};

} // namespace gui

#endif // X_WINDOW_H__

