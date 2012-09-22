#ifndef GUI_SLIDER_IMPL_H__
#define GUI_SLIDER_IMPL_H__

#include <boost/function.hpp>

#include <gui/Signals.h>
#include <gui/SliderPainter.h>
#include <pipeline/all.h>

namespace gui {

/**
 * A simple controller for a numeric value in specified range.
 */
class SliderImpl : public pipeline::SimpleProcessNode<> {

public:

	SliderImpl(double min, double max, double value);

private:

	void updateOutputs() {}

	// callback on mouse events
	void onMouseUp(MouseUp& signal);

	// callback on mouse events
	void onMouseDown(MouseDown& signal);

	// callback on mouse events
	void onMouseMove(MouseMove& signal);

	// the current value of the slider
	pipeline::Output<double> _value;

	// the painter to draw the slider
	pipeline::Output<SliderPainter> _painter;

	// the minimal value
	double _min;

	// the maximal value
	double _max;

	// indicates that the mouse is currently over the slider
	bool _mouseOver;

	// indicates that we are dragging mode
	bool _dragging;

	// the position of the slider in gl units
	float _sliderPos;

	// the width of the slider in gl units
	float _sliderWidth;

	// the the offset to the center of the slider
	float _draggingOffset;
};

} // namespace gui

#endif // GUI_SLIDER_IMPL_H__

