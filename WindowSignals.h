#ifndef GUI_WINDOW_SIGNALS_H__
#define GUI_WINDOW_SIGNALS_H__

#include "GuiSignals.h"

namespace gui {

/**
 * Base class for window signals.
 */
class WindowSignal : public GuiSignal {};

/**
 * Request to go to or leave fullscreen mode.
 */
class WindowFullscreen : public WindowSignal {

public:

	WindowFullscreen(bool fullscreen_ = true) :
		fullscreen(fullscreen_) {}

	bool fullscreen;
};

/**
 * Request to iconfiy.
 */
class WindowIconify : public WindowSignal {};

} // namespace gui

#endif // GUI_WINDOW_SIGNALS_H__

