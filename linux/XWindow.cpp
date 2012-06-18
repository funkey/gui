#include <cstdlib>

#include <X11/extensions/Xrandr.h>

#include <gui/linux/XWindow.h>
#include <gui/Modifiers.h>
#include <util/Logger.h>

using std::endl;
using std::abs;
using namespace logger;

LogChannel xlog("xlog");

namespace gui {

XWindow::XWindow(string caption, const WindowMode& mode) :
	WindowBase(caption),
	_closed(false),
	_fullscreen(false),
	_previousMode(-1) {

	// since this library was designed to be multithreaded, we feel it is safe
	// to enable X multithreading here -- even if the user does not need it
	if (!XInitThreads())
		LOG_ERROR(xlog) << "[XWindow] set up X multithreading was not successful"
		                << endl;

	LOG_ALL(xlog) << "[XWindow] setting up X server connection" << endl;

	_display = XOpenDisplay(0);
	_screen  = DefaultScreen(_display);

	if (_display == 0)
		throw "[XWindow] Unable to open display";

	// setup fullscreen -- that might change mode
	if (mode.fullscreen)
		setupFullscreen(mode);

	XSetWindowAttributes attributes;

	// register for events
	attributes.event_mask =
			FocusChangeMask | ButtonPressMask | ButtonReleaseMask |
			ButtonMotionMask | PointerMotionMask | KeyPressMask |
			KeyReleaseMask | StructureNotifyMask | EnterWindowMask |
			LeaveWindowMask | ExposureMask;

	// don't interfere with window manager in fullscreen mode
	attributes.override_redirect = _fullscreen;

	_window = XCreateWindow(
			_display,
			RootWindow(_display, _screen),
			mode.position.x, mode.position.y,
			mode.size.x, mode.size.y,
			0,
			DefaultDepth(_display, _screen),
			InputOutput,
			DefaultVisual(_display, _screen),
			CWEventMask | CWOverrideRedirect,
			&attributes);

	XStoreName(_display, _window, caption.c_str());

	LOG_ALL(xlog) << "[XWindow] registering for delete events" << endl;

	_deleteWindow = XInternAtom(_display, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(_display, _window, &_deleteWindow, 1);

	LOG_ALL(xlog) << "[XWindow] creating input context" << endl;

	_inputMethod = XOpenIM(_display, 0, 0, 0);


	if (_inputMethod) {

		_inputContext = XCreateIC(
				_inputMethod,
				XNClientWindow, _window,
				XNFocusWindow, _window,
				XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
				NULL);

	} else
		LOG_ERROR(xlog) << "[XWindow] could not create input context" << endl;

	// now we are ready to show the window
	LOG_ALL(xlog) << "[XWindow] mapping window" << endl;

	XMapWindow(_display, _window);
	XFlush(_display);

	LOG_ALL(xlog) << "[XWindow] initialized" << endl;
}

XWindow::~XWindow() {

	close();
}

void
XWindow::setupFullscreen(const WindowMode& mode) {

	int v;
	if (!XQueryExtension(_display, "RANDR", &v, &v, &v)) {

		LOG_ERROR(xlog) << "[XWindow] no xrandr extension found -- "
		                << "can not switch to fullscreen" << endl;
		return;
	}

	XRRScreenConfiguration* config =
			XRRGetScreenInfo(
					_display,
					RootWindow(_display, _screen));

	if (!config) {

		LOG_ERROR(xlog) << "[XWindow] failed to get xrandr screen configuration -- "
		                << "can not switch to fullscreen" << endl;
		return;
	}

	Rotation rotation;
	_previousMode = XRRConfigCurrentConfiguration(config, &rotation);

	// get all possible resolutions
	int numResolutions;
	XRRScreenSize* resolutions = XRRConfigSizes(config, &numResolutions);

	if (!resolutions || numResolutions == 0) {

		LOG_ERROR(xlog) << "[XWindow] failed to get possible resolutions -- "
		                << "can not switch to fullscreen" << endl;
		return;
	}

	// find the closes resolution to the desired one
	int bestResolution = -1;
	int minDifference  = 0;
	for (int i = 0; i < numResolutions; i++) {

		int width  = resolutions[i].width;
		int height = resolutions[i].height;

		int difference =
				abs(width  - mode.size.x) +
				abs(height - mode.size.y);

		if (difference < minDifference || bestResolution == -1) {

			bestResolution = i;
			minDifference  = difference;

			if (minDifference == 0)
				break;
		}
	}

	// switch to closest fullscreen resolution
	XRRSetScreenConfig(
			_display,
			config,
			RootWindow(_display, _screen),
			bestResolution,
			rotation, CurrentTime);

	XRRFreeScreenConfigInfo(config);

	_fullscreen = true;
}

void
XWindow::processEvents() {

	if (closed())
		return;

	XEvent event;

	int numEvents = XPending(_display);

	for (int i = 0; i < numEvents; i++) {

		Modifiers       modifiers;
		keys::Key       key;
		buttons::Button button;

		XNextEvent(_display, &event);

		switch (event.type) {

			case ConfigureNotify:
				// resize
				LOG_ALL(xlog) << "[XWindow] window "
				              << " received a configure notification" << endl;
				processResizeEvent(event.xconfigure.width, event.xconfigure.height);
				setDirty();
				break;

			case Expose:
				LOG_ALL(xlog) << "[XWindow] window "
				              << " received an expose notification" << endl;
				setDirty();
				break;

			case ClientMessage:
				LOG_ALL(xlog) << "[XWindow] window "
				              << " received a client message" << endl;
				if (event.xclient.data.l[0] == _deleteWindow) {

					processCloseEvent();

					// there is no need to process further events
					return;
				}

				break;

			case DestroyNotify:
				LOG_ALL(xlog) << "[XWindow] window "
				              << " received a destroy notification" << endl;

				processCloseEvent();

				// there is no need to process further events
				return;

			case KeyPress:
				LOG_ALL(xlog) << "[XWindow] window "
				              << " received a key press notification" << endl;

				key       = keycodeToKey(event.xkey.keycode);
				modifiers = stateToModifiers(event.xkey.state);

				processKeyDownEvent(key, modifiers);

				break;

			case KeyRelease:
				LOG_ALL(xlog) << "[XWindow] window "
				              << " received a key release notification" << endl;

				key       = keycodeToKey(event.xkey.keycode);
				modifiers = stateToModifiers(event.xkey.state);

				processKeyUpEvent(key, modifiers);

				break;

			case ButtonPress:
				LOG_ALL(xlog) << "[XWindow] window "
				              << " received a button press notification" << endl;

				button    = buttonToButton(event.xbutton.button);
				modifiers = stateToModifiers(event.xbutton.state);

				processButtonDownEvent(
						button,
						point<double>(event.xbutton.x, event.xbutton.y),
						modifiers);

				break;

			case ButtonRelease:
				LOG_ALL(xlog) << "[XWindow] window "
				              << " received a button release notification" << endl;

				button    = buttonToButton(event.xbutton.button);
				modifiers = stateToModifiers(event.xbutton.state);

				processButtonUpEvent(
						button,
						point<double>(event.xbutton.x, event.xbutton.y),
						modifiers);

				break;

			case MotionNotify:
				LOG_ALL(xlog) << "[XWindow] window "
				              << " received a mouse motion notification" << endl;

				modifiers = stateToModifiers(event.xbutton.state);

				processMouseMoveEvent(
						point<double>(event.xbutton.x, event.xbutton.y),
						modifiers);

				break;

			case EnterNotify:
				break;

			case LeaveNotify:
				break;

			case FocusIn:
				break;

			case FocusOut:
				break;

			case UnmapNotify:
				break;

			case MapNotify:
				break;

			default:
				LOG_ERROR(xlog) << "[XWindow] window "
				                << " received unknown event notification: "
				                << event.type << endl;
				break;
		}
	}

	// redraw only if needed
	if (isDirty() && !closed()) {

		redraw();
		setDirty(false);
	}
}

void
XWindow::close() {

	if (closed())
		return;

	_closed = true;

	// just if we changed it
	restoreVideoMode();

	// destroy input context
	if (_inputContext) {

		XDestroyIC(_inputContext);
		_inputContext = 0;
	}

	// destroy window
	if (_window) {

		XDestroyWindow(_display, _window);
		XFlush(_display);
		_window = 0;
	}

	// close input method
	if (_inputMethod) {

		XCloseIM(_inputMethod);
		_inputMethod = 0;
	}

	// close X11 connection
	XCloseDisplay(_display);
	_display = 0;
}

void
XWindow::restoreVideoMode() {

	if (!_fullscreen)
		return;

	XRRScreenConfiguration* config =
			XRRGetScreenInfo(
					_display,
					RootWindow(_display, _screen));

	if (config) {

		Rotation rotation;
		XRRConfigCurrentConfiguration(config, &rotation);

		XRRSetScreenConfig(
				_display,
				config,
				RootWindow(_display, _screen),
				_previousMode,
				rotation,
				CurrentTime);

		XRRFreeScreenConfigInfo(config);

	} else {

		LOG_ERROR(xlog) << "[XWindow] that's odd: can't reset video mode..." << endl;
	}
}

bool
XWindow::closed(){

	return _closed;
}

keys::Key
XWindow::keycodeToKey(unsigned int keycode) {

	switch (keycode) {

		case 24:
			return keys::Q;

		case 25:
			return keys::W;

		case 26:
			return keys::E;

		case 27:
			return keys::R;

		case 28:
			return keys::T;

		case 29:
			return keys::Y;

		case 30:
			return keys::U;

		case 31:
			return keys::I;

		case 32:
			return keys::O;

		case 33:
			return keys::P;

		case 38:
			return keys::A;

		case 39:
			return keys::S;

		case 40:
			return keys::D;

		case 41:
			return keys::F;

		case 42:
			return keys::G;

		case 43:
			return keys::H;

		case 44:
			return keys::J;

		case 45:
			return keys::K;

		case 46:
			return keys::L;

		case 52:
			return keys::Z;

		case 53:
			return keys::X;

		case 54:
			return keys::C;

		case 55:
			return keys::V;

		case 56:
			return keys::B;

		case 57:
			return keys::N;

		case 58:
			return keys::M;

		default:
			return keys::NoKey;
	}
}

Modifiers
XWindow::stateToModifiers(unsigned int state) {

	Modifiers modifiers;

	if (state & ControlMask)
		modifiers = static_cast<Modifiers>(modifiers | keys::ControlDown);

	if (state & Button1Mask)
		modifiers = static_cast<Modifiers>(modifiers | buttons::LeftDown);

	if (state & Button2Mask)
		modifiers = static_cast<Modifiers>(modifiers | buttons::MiddleDown);

	if (state & Button3Mask)
		modifiers = static_cast<Modifiers>(modifiers | buttons::RightDown);

	// TODO: add more modifiers

	return modifiers;
}

buttons::Button
XWindow::buttonToButton(unsigned int xbutton) {

	switch (xbutton) {

		case 1:
			return buttons::Left;

		case 2:
			return buttons::Right;

		case 3:
			return buttons::Middle;

		case 4:
			return buttons::WheelUp;

		case 5:
			return buttons::WheelDown;

		default:
			return buttons::NoButton;
	}
}

} // namespace gui
