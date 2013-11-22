#ifndef X_WINDOW_H__
#define X_WINDOW_H__

#include <map>
#include <string>

#include <X11/Xlib.h>
#include <X11/extensions/XInput2.h>

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
	 * Maximizes the window without decorations.
	 */
	void setFullscreen(bool fullscreen);

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
	 * Classification of input devices.
	 */
	enum InputType {

		Mouse,
		Pen,
		Touch
	};

	/**
	 * Enums to add, remove, or toggle window manager properties.
	 */
	enum {

		_NET_WM_STATE_REMOVE = 0,
		_NET_WM_STATE_ADD = 1,
		_NET_WM_STATE_TOGGLE = 2
	};

	/**
	 * Process a xinput2 property change event.
	 */
	void processPropertyEvent(XIPropertyEvent* propertyEvent);

	void configureTabletArea(int deviceId);

	void processPenStatusEvent(XIPropertyEvent* propertyEvent);

	/**
	 * Converts an X keycode to a Key.
	 */
	keys::Key keycodeToKey(unsigned int keycode);

	/**
	 * Converts an X state to Modifiers.
	 */
	Modifiers stateToModifiers(unsigned int state);

	/**
	 * Converts an X button to Modifiers.
	 */
	Modifiers buttonsToModifiers(XIButtonState& buttons);

	/**
	 * Converts an X button to a Button.
	 */
	buttons::Button buttonToButton(unsigned int xbutton);

	/**
	 * Determine the input type given a device id.
	 */
	InputType getInputType(int deviceid);

	/**
	 * Transform the pen device coordinates to screen coordinates according to 
	 * the calibration.
	 */
	util::point<double> getPenPosition(XIDeviceEvent* event);

	/**
	 * Assuming the input device is a pen, get the pressure level.
	 */
	double getPressure(XIDeviceEvent* event);

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

	// the current resolution of the screen
	util::point<int> _screenResolution;

	// map from device ID to input type
	std::map<int, InputType> _inputTypes;

	// list of input devices of type pen
	std::vector<int> _penDevices;

	// calibration for the pen
	double _penSlopeX;
	double _penSlopeY;
	double _penOffsetX;
	double _penOffsetY;

	// the X11 atom for the wacom id property
	Atom _serialIdsProperty;

	// the X11 atom for the wacom tablet area
	Atom _tabletAreaProperty;
};

} // namespace gui

#endif // X_WINDOW_H__

