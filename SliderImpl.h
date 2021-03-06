#ifndef GUI_SLIDER_IMPL_H__
#define GUI_SLIDER_IMPL_H__

#include <boost/function.hpp>

#include <gui/MouseSignals.h>
#include <gui/SliderPainter.h>
#include <pipeline/all.h>

namespace gui {

/**
 * A simple controller for a numeric value in specified range.
 */
template <typename Precision>
class SliderImpl : public pipeline::SimpleProcessNode<> {

public:

	SliderImpl(Precision min, Precision max, Precision value);

private:

	void updateOutputs() {}

	// callback on mouse events
	void onMouseUp(MouseUp& signal);

	// callback on mouse events
	void onMouseDown(MouseDown& signal);

	// callback on mouse events
	void onMouseMove(MouseMove& signal);

	// the current value of the slider
	pipeline::Output<Precision> _value;

	// the painter to draw the slider
	pipeline::Output<SliderPainter> _painter;

	// the minimal value
	Precision _min;

	// the maximal value
	Precision _max;

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

template <typename Precision>
SliderImpl<Precision>::SliderImpl(Precision min, Precision max, Precision value) :
	_value(boost::make_shared<Precision>(value)),
	_painter(new SliderPainter(min, max, min)),
	_min(min),
	_max(max),
	_mouseOver(false),
	_dragging(false) {

	registerOutput(_value, "value");
	registerOutput(_painter, "painter");

	_painter.registerCallback(&SliderImpl<Precision>::onMouseUp, this);
	_painter.registerCallback(&SliderImpl<Precision>::onMouseDown, this);
	_painter.registerCallback(&SliderImpl<Precision>::onMouseMove, this);

	_painter->setValue(*_value);
}

template <typename Precision>
void
SliderImpl<Precision>::onMouseUp(MouseUp& signal) {

	if (signal.button == buttons::Left) {

		// get size and location of grasp
		const util::rect<double>& graspSize = _painter->getGraspSize();

		// the mouse position
		util::point<double> pos = signal.position;

		if (!graspSize.contains(pos)) {

			_painter->setHighlight(false);
			setDirty(_painter);
		}

		_dragging = false;
	}
}

template <typename Precision>
void
SliderImpl<Precision>::onMouseDown(MouseDown& signal) {

	if (!_painter->getSize().contains(signal.position))
		return;

	// left mouse button
	if (signal.button == buttons::Left) {

		// get size and location of grasp
		const util::rect<double>& graspSize = _painter->getGraspSize();

		// the mouse position
		util::point<double> pos = signal.position;

		// within grasp
		if (graspSize.contains(pos)) {

			// the x offset to the center of the grasp
			_draggingOffset = pos.x - (graspSize.minX + graspSize.width()/2);
			_dragging       = true;

			// let the sender know that we took care of this input event
			signal.processed = true;

		// outside of grasp
		} else {

			double value = _min + (pos.x/_painter->getSize().width())*(_max - _min);
			value = std::min((double)_max, std::max((double)_min, value));

			*_value = value;

			_painter->setValue(*_value);

			setDirty(_value);
			setDirty(_painter);
		}
	}
}

template <typename Precision>
void
SliderImpl<Precision>::onMouseMove(MouseMove& signal) {

	const util::rect<double>& size = _painter->getSize();
	const util::rect<double>& graspSize = _painter->getGraspSize();

	util::point<double> pos = signal.position;

	if (size.contains(pos)) {

		double value = _min + (pos.x/size.width())*(_max - _min);
		value = std::min((double)_max, std::max((double)_min, value));

		_painter->setHoverValue(value, pos.x);
		setDirty(_painter);

	} else {

		_painter->unsetHoverValue();
		setDirty(_painter);
	}

	if (graspSize.contains(pos)) {

		if (_mouseOver == false) {

			_mouseOver = true;
			_painter->setHighlight(true);

			setDirty(_painter);
		}

	} else {

		if (_mouseOver == true) {

			_mouseOver = false;

			if (!_dragging) {

				_painter->setHighlight(false);

				setDirty(_painter);
			}
		}
	}

	if (_dragging) {

		// mouse is dragged
		if (signal.modifiers & buttons::LeftDown) {

			double pos = signal.position.x - _draggingOffset;

			double value = (double)_min + ((pos - graspSize.width()/2)/(size.width() - graspSize.width()))*(_max - _min);

			value = std::min((double)_max, std::max((double)_min, value));

			*_value = value;

			_painter->setValue(*_value);

			setDirty(_value);
			setDirty(_painter);

			// let the sender know that we took care of this input event
			signal.processed = true;
		}
	}
}

} // namespace gui

#endif // GUI_SLIDER_IMPL_H__

