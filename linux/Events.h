#ifndef GUI_EVENTS_H__
#define GUI_EVENTS_H__

#include <X11/Xlib.h>
#include <X11/keysymdef.h>

namespace gui {

enum State {

	ControlPressed = ControlMask,
	ShiftPressed   = ShiftMask
};

} // namespace gui

#endif // GUI_EVENTS_H__

