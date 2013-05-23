#include <cstdlib>
#include <ctime>

#include <X11/extensions/Xrandr.h>

#include <gui/Modifiers.h>
#include <gui/linux/XWindow.h>
#include <util/Logger.h>
#include <util/exceptions.h>

using std::endl;
using std::abs;
using namespace logger;

LogChannel xlog("xlog");

namespace gui {

XWindow::XWindow(string caption, const WindowMode& mode) :
	WindowBase(caption),
	_visible(false),
	_closed(false) {

	// By using xcb we wouldn't really need that here. However, when using xcb 
	// together with VirtualGl, calling this function is important for 
	// multithreaded operation. Since in the other cases it doesn't hurt to 
	// initialise xlibs threading support, we call it here.
	if (!XInitThreads())
		LOG_ERROR(xlog) << "[XWindow] set up X multithreading was not successful"
						<< endl;

	LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] setting up X server connection" << endl;

	// open X11 connection

	_display = XOpenDisplay(0);

	if (_display == 0)
		BOOST_THROW_EXCEPTION(GuiError() << error_message("[XWindow] Unable to open display") << STACK_TRACE);

	// get the default screen

	_screen = DefaultScreen(_display);

	// open xcb connection

	_xcbConnection = XGetXCBConnection(_display);

	if (!_xcbConnection) {

		XCloseDisplay(_display);
		BOOST_THROW_EXCEPTION(GuiError() << error_message("[XWindow] Unable to establish connection to xcb") << STACK_TRACE);
	}

	// get the event queue ownership

	XSetEventQueueOwner(_display, XCBOwnsEventQueue);

	// find xcb screen

	_xcbScreen = 0;

	xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(xcb_get_setup(_xcbConnection));

	for(int screen_num = _screen; screen_iter.rem && screen_num > 0; --screen_num, xcb_screen_next(&screen_iter));

	_xcbScreen = screen_iter.data;

	// query the framebuffer configurations

	GLXFBConfig* fbConfigs = 0;
	int numFbConfigs = 0;

	fbConfigs = glXGetFBConfigs(_display, _screen, &numFbConfigs);

	if (!fbConfigs || numFbConfigs == 0) {

		XCloseDisplay(_display);
		BOOST_THROW_EXCEPTION(GuiError() << error_message("[XWindow] unable to query frame-buffer configurations") << STACK_TRACE);
	}

	LOG_ALL(xlog) << "[XWindow] found " << numFbConfigs << " frame-buffere configurations" << std::endl;

	// select first frame-buffer configuration with the same depth as the screen

	int screenDepth = _xcbScreen->root_depth;
	int fbDepth;

	LOG_ALL(xlog) << "[XWindow] screen has a depth of " << screenDepth << std::endl;

	// try every frame-buffer configuration until one works
	for (int i = 0; i < numFbConfigs; i++) {

		glXGetFBConfigAttrib(_display, fbConfigs[i], GLX_DEPTH_SIZE, &fbDepth);

		LOG_ALL(xlog) << "[XWindow] trying frame-buffer configuration with depth of " << fbDepth << std::endl;

		_fbConfig = fbConfigs[i];

		int visualId;
		glXGetFBConfigAttrib(_display, _fbConfig, GLX_VISUAL_ID , &visualId);

		// create xcb colormap

		xcb_colormap_t colormap = xcb_generate_id(_xcbConnection);

		xcb_create_colormap(
				_xcbConnection,
				XCB_COLORMAP_ALLOC_NONE,
				colormap,
				_xcbScreen->root,
				visualId);

		// create xcb window

		_xcbWindow = xcb_generate_id(_xcbConnection);

		uint32_t eventmask =
				XCB_EVENT_MASK_EXPOSURE |
				XCB_EVENT_MASK_STRUCTURE_NOTIFY |
				XCB_EVENT_MASK_VISIBILITY_CHANGE |
				XCB_EVENT_MASK_KEY_PRESS |
				XCB_EVENT_MASK_BUTTON_PRESS |
				XCB_EVENT_MASK_BUTTON_RELEASE |
				XCB_EVENT_MASK_POINTER_MOTION;
		uint32_t valuelist[] = { eventmask, colormap, 0 };
		uint32_t valuemask = XCB_CW_EVENT_MASK | XCB_CW_COLORMAP;

		xcb_void_cookie_t createWindowCookie = xcb_create_window_checked(
				_xcbConnection,
				fbDepth,
				_xcbWindow,
				_xcbScreen->root,
				mode.position.x, mode.position.y,
				mode.size.x, mode.size.y,
				0,
				XCB_WINDOW_CLASS_INPUT_OUTPUT,
				visualId,
				valuemask,
				valuelist);

		xcb_generic_error_t* error = xcb_request_check(_xcbConnection, createWindowCookie);

		bool success = (error == 0);

		free(error);

		if (success) {

			free(error);
			break;

		} else {

			// this was the last frame-buffer configuration
			if (i == numFbConfigs - 1)
				BOOST_THROW_EXCEPTION(GuiError() << error_message("[XWindow] unable to create xcb window") << STACK_TRACE);
		}
	}

	// set window name

	xcb_change_property(_xcbConnection, XCB_PROP_MODE_REPLACE, _xcbWindow, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, caption.length(), caption.c_str());

	// register client events for close notifications

	xcb_intern_atom_cookie_t protocolsCookie = xcb_intern_atom(_xcbConnection, 1 /* if atom exists */, 12 /* size of string */, "WM_PROTOCOLS");
	xcb_intern_atom_reply_t* protocolsReply  = xcb_intern_atom_reply(_xcbConnection, protocolsCookie, 0);

	if (!protocolsReply)
		BOOST_THROW_EXCEPTION(GuiError() << error_message("couldn't access WM_PROTOCOLS") << STACK_TRACE);

	xcb_intern_atom_cookie_t deleteCookie = xcb_intern_atom(_xcbConnection, 0 /* if atom exists */, 16 /* size of string */, "WM_DELETE_WINDOW");
	_deleteReply                          = xcb_intern_atom_reply(_xcbConnection, deleteCookie, 0);

	if (!_deleteReply)
		BOOST_THROW_EXCEPTION(GuiError() << error_message("couldn't access WM_DELETE_WINDOW") << STACK_TRACE);

	xcb_change_property(
			_xcbConnection,
			XCB_PROP_MODE_REPLACE,
			_xcbWindow,
			(*protocolsReply).atom, /* the property to change */
			XCB_ATOM_ATOM,          /* type of the property */
			32,                     /* data element format */
			1,                      /* number of elements in data */
			&(*_deleteReply).atom); /* data */

	free(protocolsReply);

	// map window

	if (mode.mapped)
		xcb_map_window(_xcbConnection, _xcbWindow); 

	xcb_flush(_xcbConnection);

	LOG_DEBUG(xlog) << "[XWindow] [" << getCaption() << "] initialized" << endl;
}

XWindow::~XWindow() {

	LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] destructing" << endl;

	// clsoe the window (if this didn't happen before already)

	close();

	// free the atom reply for delete events

	free(_deleteReply);

	// close X11 connection

	XCloseDisplay(_display);

	LOG_DEBUG(xlog) << "[XWindow] [" << getCaption() << "] destructed" << endl;
}

void
XWindow::processEvents() {

	if (closed())
		return;

	LOG_DEBUG(xlog) << "[XWindow] [" << getCaption() << "] entering event loop" << endl;

	xcb_generic_event_t *event;

	std::clock_t timer = std::clock();

	while (!closed()) {

		// get the number of elapsed microseconds
		double elapsed = static_cast<double>(std::clock() - timer)/static_cast<double>(CLOCKS_PER_SEC);
		int microsElapsed = static_cast<int>(elapsed*10e6);

		// reset timer
		timer = std::clock();

		// wait until at least 100 microseconds have passed since the last event 
		// poll
		int sleepMicros = std::max(0, 100 - microsElapsed);

		//LOG_ALL(xlog) << "sleeping for " << sleepMicros << " micros" << std::endl;
		usleep(sleepMicros);

		// poll for events
		while (event = xcb_poll_for_event(_xcbConnection)) {

			LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] got an event" << endl;

			Modifiers       modifiers;
			keys::Key       key;
			buttons::Button button;

			switch(event->response_type & ~0x80) {

				case XCB_MAP_NOTIFY:

					{
						LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
									  << "received a map notification" << endl;

						_visible = true;
					}

					break;

				case XCB_UNMAP_NOTIFY:

					{
						LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
									  << "received an unmap notification" << endl;

						_visible = false;
					}

					break;

				case XCB_CONFIGURE_NOTIFY:

					{
						LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
									  << "received a configure notification" << endl;

						xcb_configure_notify_event_t* configureEvent = (xcb_configure_notify_event_t*)event;

						processResizeEvent(configureEvent->width, configureEvent->height);
					}

					break;

				case XCB_EXPOSE:

					{
						LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
									  << "received an expose notification" << endl;

						boost::mutex::scoped_lock lock(getDirtyMutex());

						setDirty();
					}

					break;

				case XCB_VISIBILITY_NOTIFY:

					{
						LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
									  << "received a visibility notification" << endl;

						xcb_visibility_notify_event_t* visibilityEvent = (xcb_visibility_notify_event_t*)event;

						switch (visibilityEvent->state) {

							case  XCB_VISIBILITY_UNOBSCURED:

								LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
											  << "I am entirely visible" << endl;

								_visible = true;

								break;

							case XCB_VISIBILITY_PARTIALLY_OBSCURED:

								LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
											  << "I am partially visible" << endl;

								_visible = true;

								break;

							case XCB_VISIBILITY_FULLY_OBSCURED:

								LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
											  << "I am not visible" << endl;

								_visible = false;

								break;

							default:

								LOG_ERROR(xlog) << "[XWindow] [" << getCaption() << "] "
												<< "unknown visibility type " << (unsigned int)visibilityEvent->state << endl;
						}
					}

					break;

				case XCB_CLIENT_MESSAGE:

					{
						LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
									  << "received a client message" << endl;

						xcb_client_message_event_t* clientEvent = (xcb_client_message_event_t*)event;

						if (clientEvent->data.data32[0] == _deleteReply->atom) {

							LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
										  << "client requested window deletion" << endl;

							processCloseEvent();
						}
					}

					break;

				case XCB_KEY_PRESS:

					{
						LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
									  << "received a key press notification" << endl;

						xcb_key_press_event_t* pressEvent = (xcb_key_press_event_t*)event;

						key       = keycodeToKey(pressEvent->detail);
						modifiers = stateToModifiers(pressEvent->state);

						processKeyDownEvent(key, modifiers);
					}

					break;

				case KeyRelease:

					{
						LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
									  << "received a key release notification" << endl;

						xcb_key_release_event_t* releaseEvent = (xcb_key_release_event_t*)event;

						key       = keycodeToKey(releaseEvent->detail);
						modifiers = stateToModifiers(releaseEvent->state);

						processKeyUpEvent(key, modifiers);
					}

					break;

				case XCB_BUTTON_PRESS:

					{
						LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
									  << " received a button press notification" << endl;

						xcb_button_press_event_t* pressEvent = (xcb_button_press_event_t*)event;

						button    = buttonToButton(pressEvent->detail);
						modifiers = stateToModifiers(pressEvent->state);

						processButtonDownEvent(
								button,
								point<double>(pressEvent->event_x, pressEvent->event_y),
								modifiers);
					}

					break;

				case XCB_BUTTON_RELEASE:

					{
						LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
									  << " received a button release notification" << endl;

						xcb_button_release_event_t* releaseEvent = (xcb_button_release_event_t*)event;

						button    = buttonToButton(releaseEvent->detail);
						modifiers = stateToModifiers(releaseEvent->state);

						processButtonUpEvent(
								button,
								point<double>(releaseEvent->event_x, releaseEvent->event_y),
								modifiers);
					}

					break;

				case XCB_MOTION_NOTIFY:

					{
						LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] "
									  << " received a mouse motion notification" << endl;

						xcb_motion_notify_event_t* motionEvent = (xcb_motion_notify_event_t*)event;

						modifiers = stateToModifiers(motionEvent->state);

						processMouseMoveEvent(
								point<double>(motionEvent->event_x, motionEvent->event_y),
								modifiers);
					}

					break;

				default:

					LOG_ERROR(xlog) << "[XWindow] [" << getCaption() << "] "
									<< "received unknown event notification: "
									<< (unsigned int)event->response_type << endl;
					break;
			}

			free(event);

		} // polling for events

		boost::mutex::scoped_lock lock(getDirtyMutex());

		// redraw only if needed
		if (isDirty() && !closed() && _visible) {

			setDirty(false);

			lock.unlock();

			LOG_ALL(xlog) << "[XWindow] [" << getCaption() << "] redraw requested" << endl;

			redraw();
		}
	}
}

void
XWindow::close() {

	if (closed())
		return;

	_closed = true;

	// destroy the window

	xcb_destroy_window(_xcbConnection, _xcbWindow);
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

	if (state & XCB_MOD_MASK_CONTROL)
		modifiers = static_cast<Modifiers>(modifiers | keys::ControlDown);

	if (state & XCB_MOD_MASK_SHIFT)
		modifiers = static_cast<Modifiers>(modifiers | keys::ShiftDown);

	if (state & XCB_MOD_MASK_1)
		modifiers = static_cast<Modifiers>(modifiers | keys::AltDown);

	if (state & XCB_BUTTON_MASK_1)
		modifiers = static_cast<Modifiers>(modifiers | buttons::LeftDown);

	if (state & XCB_BUTTON_MASK_2)
		modifiers = static_cast<Modifiers>(modifiers | buttons::MiddleDown);

	if (state & XCB_BUTTON_MASK_3)
		modifiers = static_cast<Modifiers>(modifiers | buttons::RightDown);

	// TODO: add more modifiers

	return modifiers;
}

buttons::Button
XWindow::buttonToButton(const xcb_button_t& button) {

	switch (button) {

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
