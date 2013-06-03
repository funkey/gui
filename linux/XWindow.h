#ifndef X_WINDOW_H__
#define X_WINDOW_H__

#include <string>

#include <X11/Xlib.h>

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
	 * Provides access to the X11 display to create an OpenGl context.
	 */
	Display* getDisplay() { return _display; };

	/**
	 * Provides access to the X11 window to create an OpenGl context.
	 */
	::Window getX11Window() { return _window; };

private:

	/**
	 * Finds the closes fullscreen resolution to the one given in mode and
	 * switches to it.
	 */
	void setupFullscreen(const WindowMode& mode);

	/**
	 * Restore the video mode that we had before we changed it.
	 */
	void restoreVideoMode();

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
	buttons::Button buttonToButton(unsigned int xbutton);

	// the X11 display
	Display* _display;

	// the X11 screen
	int      _screen;

	// the X11 window
	::Window _window;

	// the X11 atom for delete window events
	Atom     _deleteWindow;

	// the X11 input method
	XIM      _inputMethod;

	// the X11 input context
	XIC      _inputContext;

	// the opcode for xinput2 events
	int      _xinputOpcode;

	// was closed
	bool     _closed;

	// is this window running in fullscreen?
	bool     _fullscreen;

	// the video mode before the swith to fullscreen
	int      _previousMode;
};

} // namespace gui

#endif // X_WINDOW_H__

