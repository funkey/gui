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
class SwitchImpl : public pipeline::SimpleProcessNode<> {

public:

	SwitchImpl(bool value);

private:

	void updateOutputs();

	// callback on mouse events
	void onMouseUp(MouseUp& signal);

	// callback on mouse events
	void onMouseMove(MouseMove& signal);

	// the current value of the switch
	pipeline::Output<bool> _value;

	// the painter to draw the switch
	pipeline::Output<SwitchPainter> _painter;

	// indicates that the mouse is currently over the switch
	bool _mouseOver;
};

} // namespace gui

#endif // GUI_SWITCH_IMPL_H__

