#include <util/Logger.h>
#include "ZoomView.h"

namespace gui {

static logger::LogChannel zoomviewlog("zoomviewlog", "[ZoomView] ");

ZoomView::ZoomView() :
		_scale(1.0),
		_shift(0.0, 0.0),
		_zoomStep(1.1),
		_dragging(false) {

	registerInput(_content, "painter");
	registerOutput(_zoomed, "painter");

	_content.registerBackwardSlot(_update);
	_content.registerBackwardSlot(_keyDown);
	_content.registerBackwardSlot(_keyUp);
	_content.registerBackwardSlot(_mouseDown);
	_content.registerBackwardSlot(_mouseUp);
	_content.registerBackwardSlot(_mouseMove);
	_content.registerBackwardCallback(&ZoomView::onInputSet, this);
	_content.registerBackwardCallback(&ZoomView::onModified, this);
	_content.registerBackwardCallback(&ZoomView::onContentChanged, this);
	_content.registerBackwardCallback(&ZoomView::onSizeChanged, this);

	_zoomed.registerForwardSlot(_modified);
	_zoomed.registerForwardSlot(_contentChanged);
	_zoomed.registerForwardSlot(_sizeChanged);
	_zoomed.registerForwardCallback(&ZoomView::onUpdate, this);
	_zoomed.registerForwardCallback(&ZoomView::onKeyUp, this);
	_zoomed.registerForwardCallback(&ZoomView::onKeyDown, this);
	_zoomed.registerForwardCallback(&ZoomView::onMouseUp, this);
	_zoomed.registerForwardCallback(&ZoomView::onMouseDown, this);
	_zoomed.registerForwardCallback(&ZoomView::onMouseMove, this);
}

void
ZoomView::onInputSet(const pipeline::InputSet<Painter>& signal) {

	LOG_ALL(zoomviewlog) << "got a new painter" << std::endl;

	if (!_zoomed)
		_zoomed.createData();

	_zoomed->setContent(_content);

	_modified();
}

void
ZoomView::onModified(const pipeline::Modified& signal) {

	// just pass this signal on
	_modified();
}

void
ZoomView::onContentChanged(const ContentChanged& signal) {

	_contentChanged();
}

void
ZoomView::onSizeChanged(const SizeChanged& signal) {

	_zoomed->updateSize();

	_sizeChanged(SizeChanged(_zoomed->getSize()));
}

void
ZoomView::onUpdate(const pipeline::Update& signal) {

	_update(signal);

	_zoomed->setScale(_scale);
	_zoomed->setShift(_shift);
}

void
ZoomView::onKeyUp(const KeyUp& signal) {

	// pass on the signal
	_keyUp(signal);
}

void
ZoomView::onKeyDown(KeyDown& signal) {

	LOG_ALL(zoomviewlog) << "a key was pressed" << std::endl;

	if (signal.key == keys::R) {

		LOG_ALL(zoomviewlog) << "resetting scale and shift" << std::endl;

		_scale = 1.0;
		_shift = util::point<double>(0.0, 0.0);

		_modified();

		signal.processed = true;

	} else {

		_keyDown(signal);
	}
}

void
ZoomView::onMouseUp(const MouseUp& signal) {

	LOG_ALL(zoomviewlog) << "a button was released" << std::endl;

	MouseUp zoomedSignal = signal;
	zoomedSignal.position /= _scale;
	zoomedSignal.position -= _shift;

	_mouseUp(zoomedSignal);
}

void
ZoomView::onMouseDown(const MouseDown& signal) {

	LOG_ALL(zoomviewlog) << "a button was pressed" << std::endl;

	MouseDown zoomedSignal = signal;
	zoomedSignal.position /= _scale;
	zoomedSignal.position -= _shift;

	if (!(signal.modifiers & keys::ControlDown)) {

		_mouseDown(zoomedSignal);
		return;
	}

	util::point<double> position = signal.position;

	LOG_ALL(zoomviewlog) << "mouse button " << signal.button << " down, position is " << position << std::endl;

	if (signal.button == buttons::Left) {

		LOG_ALL(zoomviewlog) << "it's the left mouse button -- start dragging mode" << std::endl;

		_dragging = true;
		_buttonDown = position;

		return;
	}

	// in the following, treat x and y zoom-corrected:
	position.x /= _scale;
	position.y /= _scale;

	// if shift is pressed, increase zoom speed
	double zoomStep = _zoomStep;
	if (signal.modifiers & keys::ShiftDown)
		zoomStep *= 2;

	// mouse wheel up
	if (signal.button == buttons::WheelUp) {

		LOG_ALL(zoomviewlog) << "it's the left wheel up" << std::endl;

		_scale *= zoomStep;
		_shift += position*(1.0/zoomStep - 1.0);

		LOG_ALL(zoomviewlog) << "mouse wheel up, new zoom "
							 << _scale << ", shift " << _shift
							 << ", position was " << position << std::endl;
	}

	// mouse wheel down
	if (signal.button == buttons::WheelDown) {

		LOG_ALL(zoomviewlog) << "it's the left wheel down" << std::endl;

		_scale *= 1.0/zoomStep;
		_shift += position*(zoomStep - 1.0);

		LOG_ALL(zoomviewlog) << "mouse wheel down, new zoom "
							 << _scale << ", shift " << _shift
							 << ", position was " << position << std::endl;
	}

	_modified();
}

void
ZoomView::onMouseMove(const MouseMove& signal) {

	LOG_ALL(zoomviewlog) << "the mouse is moved" << std::endl;

	MouseMove zoomedSignal = signal;
	zoomedSignal.position /= _scale;
	zoomedSignal.position -= _shift;


	if (!(signal.modifiers & keys::ControlDown)) {

		_mouseMove(zoomedSignal);
		return;
	}

	if (!_dragging) {

		return;
	}

	LOG_ALL(zoomviewlog) << "I am in dragging mode" << std::endl;

	double amp = 1.0;
	if (signal.modifiers & keys::ShiftDown)
		amp = 10.0;

	// mouse is dragged
	if (signal.modifiers & buttons::LeftDown) {

		LOG_ALL(zoomviewlog) << "left button is still pressed" << std::endl;

		util::point<double> moved = signal.position - _buttonDown;

		_shift += moved*amp/_scale;

		_buttonDown = signal.position;

		_zoomed->setShift(_shift);

		_modified();

	} else {

		LOG_ALL(zoomviewlog) << "left button released -- stop dragging" << std::endl;

		_dragging = false;
	}
}

} // namespace gui
