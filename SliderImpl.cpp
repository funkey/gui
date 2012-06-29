#include <util/Logger.h>
#include "SliderImpl.h"

using namespace logger;

LogChannel sliderlog("sliderlog", "");

namespace gui {

SliderImpl::SliderImpl(double min, double max, double value) :
	_value(value),
	_painter(new SliderPainter(min, max, min)),
	_min(min),
	_max(max),
	_mouseOver(false),
	_dragging(false) {

	registerOutput(_value, "value");
	registerOutput(_painter, "painter");

	_painter.registerForwardCallback(&SliderImpl::onMouseUp, this);
	_painter.registerForwardCallback(&SliderImpl::onMouseDown, this);
	_painter.registerForwardCallback(&SliderImpl::onMouseMove, this);

	_painter->setValue(*_value);
}

void
SliderImpl::onMouseUp(MouseUp& signal) {

	if (signal.button == buttons::Left) {

		LOG_ALL(sliderlog) << "left button up" << std::endl;

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

void
SliderImpl::onMouseDown(MouseDown& signal) {

	// left mouse button
	if (signal.button == buttons::Left) {

		// get size and location of grasp
		const util::rect<double>& graspSize = _painter->getGraspSize();

		// the mouse position
		util::point<double> pos = signal.position;

		LOG_ALL(sliderlog) << "left button down at " << pos << std::endl;

		// within slider
		if (graspSize.contains(pos)) {

			LOG_ALL(sliderlog) << "starting dragging mode" << std::endl;

			// the x offset to the center of the grasp
			_draggingOffset = pos.x - (graspSize.minX + graspSize.width()/2);
			_dragging       = true;

			// let the sender know that we took care of this input event
			signal.processed = true;
		}
	}
}

void
SliderImpl::onMouseMove(MouseMove& signal) {

	const util::rect<double>& size = _painter->getSize();
	const util::rect<double>& graspSize = _painter->getGraspSize();

	util::point<double> pos = signal.position;

	LOG_ALL(sliderlog) << "mouse moved at " << pos << ", my grasp is at " << graspSize << std::endl;

	if (graspSize.contains(pos)) {

		LOG_ALL(sliderlog) << "hihi" << std::endl;

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

			LOG_ALL(sliderlog) << "button pressed at " << signal.position.x << std::endl;

			double pos = signal.position.x - _draggingOffset;

			double value = _min + ((pos - graspSize.width()/2)/(size.width() - graspSize.width()))*(_max - _min);

			LOG_ALL(sliderlog) << "requested value is " << value << std::endl;

			value = std::min(_max, std::max(_min, value));

			LOG_ALL(sliderlog) << "new value will be " << value << std::endl;

			LOG_ALL(sliderlog) << "value was " << *_value << std::endl;

			*_value = value;

			LOG_ALL(sliderlog) << "value is " << *_value << std::endl;

			_painter->setValue(*_value);

			setDirty(_value);
			setDirty(_painter);

			// let the sender know that we took care of this input event
			signal.processed = true;
		}
	}
}

} // namespace gui
