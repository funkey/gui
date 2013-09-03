#include <cstdlib>

#include <boost/timer/timer.hpp>

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
	_previousMode(-1),
	_penSlopeX(0.07459),
	_penSlopeY(0.07438),
	_penOffsetX(0.053229),
	_penOffsetY(-0.000444) {

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

	// init xinput2

	int event, error;
	if (!XQueryExtension(_display, "XInputExtension", &_xinputOpcode, &event, &error)) {

		LOG_ERROR(xlog) << "[XWindow] [" << getCaption() << "] no xinput extension available!" << std::endl;
	}

	// query version of xinput
	int major = 2, minor = 0;
	if (XIQueryVersion(_display, &major, &minor) == BadRequest) {

		LOG_ERROR(xlog) << "[XWindow] [" << getCaption() << "] XI2 not available. Server supports " << major << "." << minor << std::endl;
	}

	// register for events
	XIEventMask eventmask;

	eventmask.deviceid = XIAllDevices;
	eventmask.mask_len = XIMaskLen(XI_LASTEVENT);
	eventmask.mask = (unsigned char*)calloc(eventmask.mask_len, sizeof(unsigned char));
	/* now set the mask */
	XISetMask(eventmask.mask, XI_TouchBegin);
	XISetMask(eventmask.mask, XI_TouchUpdate);
	XISetMask(eventmask.mask, XI_TouchEnd);
	XISetMask(eventmask.mask, XI_ButtonPress);
	XISetMask(eventmask.mask, XI_ButtonRelease);
	XISetMask(eventmask.mask, XI_Motion);
	XISetMask(eventmask.mask, XI_PropertyEvent);

	/* select on the window */
	XISelectEvents(_display, _window, &eventmask, 1);

	// get the X atom for Wacom serial ID properties -- they tell us whether the 
	// pen is in proximity
	_serialIdsProperty = XInternAtom(_display, "Wacom Serial IDs", false);

	if (mode.hideCursor) {

		// hide the cursor
		Cursor invisibleCursor;
		Pixmap bitmapNoData;
		XColor black;
		static char noData[] = { 0,0,0,0,0,0,0,0 };
		black.red = black.green = black.blue = 0;

		bitmapNoData = XCreateBitmapFromData(_display, _window, noData, 8, 8);
		invisibleCursor = XCreatePixmapCursor(
				_display,
				bitmapNoData,
				bitmapNoData,
				&black,
				&black,
				0, 0);
		XDefineCursor(_display, _window, invisibleCursor);
		XFreeCursor(_display, invisibleCursor);
	}

	// now we are ready to show the window
	LOG_ALL(xlog) << "[XWindow] mapping window" << endl;

	XMapWindow(_display, _window);
	XFlush(_display);

	free(eventmask.mask);

	LOG_ALL(xlog) << "[XWindow] initialized" << endl;

	// setup fullscreen
	setFullscreen(mode.fullscreen);
}

XWindow::~XWindow() {

	LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] destructing..." << std::endl;

	close();

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

	LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] destructed" << std::endl;
}

void
XWindow::setFullscreen(bool fullscreen) {

	Atom wmState = XInternAtom(_display, "_NET_WM_STATE", false);
	Atom fullscreenAtom = XInternAtom(_display, "_NET_WM_STATE_FULLSCREEN", false);

	XEvent event;

	event.xclient.type = ClientMessage;
	event.xclient.serial = 0;
	event.xclient.send_event = True;
	event.xclient.window = _window;
	event.xclient.message_type = wmState;
	event.xclient.format = 32;
	event.xclient.data.l[0] = (fullscreen ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE);
	event.xclient.data.l[1] = fullscreenAtom;
	event.xclient.data.l[2] = 0;

	XSendEvent(_display, DefaultRootWindow(_display), false, SubstructureRedirectMask | SubstructureNotifyMask, &event);

	_fullscreen = fullscreen;
}

void
XWindow::processEvents() {

	if (closed())
		return;

	bool busy = false;

	boost::timer::cpu_timer timer;

	const boost::timer::nanosecond_type NanosBusyWait = 1000000LL;  // 1/1000th of a second
	const boost::timer::nanosecond_type NanosIdleWait = 10000000LL; // 1/100th of a second

	while (!closed()) {

		XEvent event;

		int numEvents = XPending(_display);

		busy = (numEvents > 0);

		for (int i = 0; i < numEvents; i++) {

			Modifiers       modifiers;
			keys::Key       key;
			buttons::Button button;

			XNextEvent(_display, &event);

			if (event.xcookie.type == GenericEvent &&
				event.xcookie.extension == _xinputOpcode &&
				XGetEventData(_display, &event.xcookie)) {

				if (event.xcookie.evtype == XI_PropertyEvent) {

					processPropertyEvent((XIPropertyEvent*)event.xcookie.data);
					continue;
				}

				XIDeviceEvent* deviceEvent = (XIDeviceEvent*)event.xcookie.data;
				double* val = deviceEvent->valuators.values;

				InputType inputType = getInputType(deviceEvent->deviceid);

				LOG_ALL(xlog) << "[XWindow] event dump:" << std::endl;
				LOG_ALL(xlog) << "[XWindow] \tdevice " << deviceEvent->deviceid << " (" << getInputType(deviceEvent->deviceid) << ") , source " << deviceEvent->sourceid  << std::endl;
				LOG_ALL(xlog) << "[XWindow] \tevent coordinates: " << deviceEvent->event_x << ", " << deviceEvent->event_y << std::endl;
				LOG_ALL(xlog) << "[XWindow] \troot  coordinates: " << deviceEvent->root_x  << ", " << deviceEvent->root_y << std::endl;
				LOG_ALL(xlog) << "[XWindow] \tdetail: " << deviceEvent->detail << std::endl;
				for (int i = 0; i < deviceEvent->valuators.mask_len * 8; i++)
					if (XIMaskIsSet(deviceEvent->valuators.mask, i))
						LOG_ALL(xlog) << "[XWindow] \t" << i << ": " << *val++ << std::endl;

				switch (event.xcookie.evtype) {

					case XI_TouchBegin:

						if (inputType == Touch) {

							LOG_ALL(xlog) << "[XWindow] finger down" << std::endl;

							processFingerDownEvent(
										deviceEvent->time,
										button,
										point<double>(deviceEvent->event_x, deviceEvent->event_y),
										deviceEvent->detail,
										modifiers);
						}

						break;

					case XI_TouchUpdate:

						if (inputType == Touch) {

							LOG_ALL(xlog) << "[XWindow] finger moved" << std::endl;

							processFingerMoveEvent(
										deviceEvent->time,
										point<double>(deviceEvent->event_x, deviceEvent->event_y),
										deviceEvent->detail,
										modifiers);
						}

						break;

					case XI_TouchEnd:

						if (inputType == Touch) {

							LOG_ALL(xlog) << "[XWindow] finger up" << std::endl;

							processFingerUpEvent(
										deviceEvent->time,
										button,
										point<double>(deviceEvent->event_x, deviceEvent->event_y),
										deviceEvent->detail,
										modifiers);
						}

						break;

					case XI_ButtonPress:

						button    = buttonToButton(deviceEvent->detail);
						modifiers = stateToModifiers(deviceEvent->mods.base | deviceEvent->mods.locked);

						if (inputType == Mouse) {

							processButtonDownEvent(
									deviceEvent->time,
									button,
									point<double>(deviceEvent->event_x, deviceEvent->event_y),
									modifiers);

						} else if (inputType == Pen) {

							processPenDownEvent(
									deviceEvent->time,
									button,
									getPenPosition(deviceEvent),
									getPressure(deviceEvent),
									modifiers);
						}

						break;

					case XI_ButtonRelease:

						button    = buttonToButton(deviceEvent->detail);
						modifiers = stateToModifiers(deviceEvent->mods.base | deviceEvent->mods.locked);

						if (inputType == Mouse) {

							processButtonUpEvent(
									deviceEvent->time,
									button,
									point<double>(deviceEvent->event_x, deviceEvent->event_y),
									modifiers);

						} else if (inputType == Pen) {

							processPenUpEvent(
									deviceEvent->time,
									button,
									getPenPosition(deviceEvent),
									getPressure(deviceEvent),
									modifiers);
						}

						break;

					case XI_Motion:

						modifiers = static_cast<Modifiers>(stateToModifiers(deviceEvent->mods.base | deviceEvent->mods.locked) | buttonsToModifiers(deviceEvent->buttons));

						if (inputType == Mouse) {

							processMouseMoveEvent(
									deviceEvent->time,
									point<double>(deviceEvent->event_x, deviceEvent->event_y),
									modifiers);

						} else if (inputType == Pen) {

							processPenMoveEvent(
									deviceEvent->time,
									getPenPosition(deviceEvent),
									getPressure(deviceEvent),
									modifiers);
						}

						break;

					default:

						LOG_ALL(xlog) << "[XWindow] received unknown xinput2 event" << std::endl;
						break;
				}

			} else {

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

						if (key == gui::keys::F)
							setFullscreen(!_fullscreen);
						else
							processKeyUpEvent(key, modifiers);

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
		}

		// redraw only if needed
		if (isDirty() && !closed()) {

			setDirty(false);

			redraw();

			busy = true;
		}

		boost::timer::cpu_times const elapsed(timer.elapsed());

		boost::timer::nanosecond_type waitAtLeast = (busy ? NanosBusyWait : NanosIdleWait);

		if (elapsed.wall <= waitAtLeast)
			usleep((waitAtLeast - elapsed.wall)/1000);

		timer.stop();
		timer.start();
	}
}

void
XWindow::processPropertyEvent(XIPropertyEvent* propertyEvent) {

	LOG_ALL(xlog) << "device " << propertyEvent->deviceid << " changed" << std::endl;
	LOG_ALL(xlog) << "\tatom: " << XGetAtomName(_display, propertyEvent->property) << ", what: " << propertyEvent->what << std::endl;

	// what == 0 is set when the pen was put back into the tablet
	if (propertyEvent->what == 0) {

		processPenAwayEvent(propertyEvent->time);
		return;
	}

	// we're only interested in the serial IDs
	if (propertyEvent->property != _serialIdsProperty)
		return;

	// get the property data
	unsigned char* data;
	Atom actualType;
	int actualFormat;
	unsigned long numItems, bytesAfter;
	if (!XIGetProperty(
			_display,
			propertyEvent->deviceid,
			propertyEvent->property,
			0,     /* offset */
			1000,  /* length */
			false, /* delete property */
			AnyPropertyType,
			&actualType,
			&actualFormat,
			&numItems,
			&bytesAfter,
			&data) == Success) {

		LOG_ERROR(xlog) << "couldn't read property " << XGetAtomName(_display, propertyEvent->property) << std::endl;
		return;
	}

	// the third number is the current serial number -- 0, if no pen present
	unsigned char* ptr = data + actualFormat/8 * 3;
	int currentSerialNumber = *((int32_t*)ptr);

	if (currentSerialNumber == 0)
		processPenOutEvent(propertyEvent->time);
	else
		processPenInEvent(propertyEvent->time);

	// show all the other items
	//for (int i = 0; i < numItems; i++) {

		//ptr = data + actualFormat/8 * i;
		//int d = *((int32_t*)ptr);
		//LOG_ALL(xlog) << "property data item " << i << ": " << d << std::endl;
	//}
}

void
XWindow::close() {

	if (closed())
		return;

	_closed = true;

	LOG_DEBUG(xlog) << "unmapping window " << getCaption() << std::endl;

	XUnmapWindow(_display, _window);
	XFlush(_display);
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

	Modifiers modifiers = NoModifier;

	if (state & ControlMask)
		modifiers = static_cast<Modifiers>(modifiers | keys::ControlDown);

	// TODO: add more modifiers

	return modifiers;
}

Modifiers
XWindow::buttonsToModifiers(XIButtonState& buttons) {

	Modifiers modifiers = NoModifier;

	for (int i = 0; i <= 3; i++) {

		if ((buttons.mask[(i)>>3] & (1 << ((i)&7)))) {

			switch(i) {

				case 1:
					modifiers = static_cast<Modifiers>(modifiers | buttons::LeftDown);
					break;

				case 2:
					modifiers = static_cast<Modifiers>(modifiers | buttons::MiddleDown);
					break;

				case 3:
					modifiers = static_cast<Modifiers>(modifiers | buttons::RightDown);
					break;
			}
		}
	}

	return modifiers;
}

buttons::Button
XWindow::buttonToButton(unsigned int xbutton) {

	switch (xbutton) {

		case 1:
			return buttons::Left;

		case 2:
			return buttons::Middle;

		case 3:
			return buttons::Right;

		case 4:
			return buttons::WheelUp;

		case 5:
			return buttons::WheelDown;

		default:
			return buttons::NoButton;
	}
}

XWindow::InputType
XWindow::getInputType(int deviceid) {

	std::map<int, InputType>::iterator r = _inputTypes.find(deviceid);

	if (r != _inputTypes.end())
		return r->second;

	int numFound;
	XIDeviceInfo* info = XIQueryDevice(_display, deviceid, &numFound);

	for (int i = 0; i < numFound; i++) {

		if (strcasestr(info[i].name, "touch")) {

			LOG_DEBUG(xlog) << "found a new input device (" << deviceid << ") of type Touch" << std::endl;

			_inputTypes[deviceid] = Touch;
			XIFreeDeviceInfo(info);

			return Touch;

		} else if (strcasestr(info[i].name, "pen")) {

			LOG_DEBUG(xlog) << "found a new input device (" << deviceid << ") of type Pen" << std::endl;

			_inputTypes[deviceid] = Pen;
			XIFreeDeviceInfo(info);

			return Pen;
		}
	}

	LOG_DEBUG(xlog) << "found a new input device (" << deviceid << ") of type Mouse" << std::endl;

	// default
	_inputTypes[deviceid] = Mouse;
	XIFreeDeviceInfo(info);

	return Mouse;
}

util::point<double>
XWindow::getPenPosition(XIDeviceEvent* event) {

	util::point<double> position;

	position.x = event->valuators.values[0]*_penSlopeX + _penOffsetX;
	position.y = event->valuators.values[1]*_penSlopeY + _penOffsetY;

	return position;
}

double
XWindow::getPressure(XIDeviceEvent* event) {

	const unsigned int pressureIndex = 2;

	if (XIMaskIsSet(event->valuators.mask, pressureIndex))
		return event->valuators.values[2];

	return 0.75;
}

} // namespace gui
