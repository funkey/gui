#ifndef GUI_SWITCH_IMPL_H__
#define GUI_SWITCH_IMPL_H__

#include <boost/function.hpp>

#include <gui/Signals.h>
#include <gui/SwitchPainter.h>
#include <pipeline/all.h>

namespace gui {

/**
 * A simple controller for a boolean value.
 */
class SwitchImpl : public pipeline::ProcessNode {

public:

	SwitchImpl();

private:

	// callback on mouse events
	void onMouseUp(MouseUp& signal);

	// callback on mouse events
	void onMouseMove(MouseMove& signal);

	// the current value of the switch
	pipeline::Output<bool> _value;

	// the painter to draw the switch
	pipeline::Output<SwitchPainter> _painter;

	// signal to send when the value was changed
	signals::Slot<pipeline::Modified> _valueModified;

	// signal to send when the painter was changed
	signals::Slot<pipeline::Modified> _painterModified;

	// indicates that the mouse is currently over the switch
	bool _mouseOver;
};

} // namespace gui

#endif // GUI_SWITCH_IMPL_H__

