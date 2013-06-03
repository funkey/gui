#include <util/Logger.h>
#include "ZoomView.h"

namespace gui {

static logger::LogChannel zoomviewlog("zoomviewlog", "[ZoomView] ");

ZoomView::ZoomView(bool autoscale) :
		_zoomed(boost::make_shared<ZoomPainter>()),
		_zoomStep(1.1),
		_autoscale(autoscale) {

	registerInput(_content, "painter");
	registerOutput(_zoomed, "painter");

	_content.registerBackwardSlot(_keyDown);
	_content.registerBackwardSlot(_keyUp);
	_content.registerBackwardCallback(&ZoomView::onInputSet, this);
	_content.registerBackwardCallback(&ZoomView::onContentChanged, this);
	_content.registerBackwardCallback(&ZoomView::onSizeChanged, this);

	_zoomed.registerForwardSlot(_contentChanged);
	_zoomed.registerForwardSlot(_sizeChanged);
	_zoomed.registerForwardCallback(&ZoomView::onKeyUp, this);
	_zoomed.registerForwardCallback(&ZoomView::onKeyDown, this);
	_zoomed.registerForwardCallback(&ZoomView::onFingerDown, this);
	_zoomed.registerForwardCallback(&ZoomView::onFingerMove, this);

	// establish pointer signal filter
	PointerSignalFilter::filterBackward(_zoomed, _content, this);

	if (_autoscale) {

		_zoomed->setAutoscale();
		_zoomed.registerForwardCallback(&ZoomView::onResize, this);
	}
}

ZoomView::ZoomView(const util::rect<double>& desiredSize) :
		_zoomed(boost::make_shared<ZoomPainter>()),
		_zoomStep(1.1),
		_autoscale(true) {

	registerInput(_content, "painter");
	registerOutput(_zoomed, "painter");

	_content.registerBackwardSlot(_keyDown);
	_content.registerBackwardSlot(_keyUp);
	_content.registerBackwardCallback(&ZoomView::onInputSet, this);
	_content.registerBackwardCallback(&ZoomView::onContentChanged, this);
	_content.registerBackwardCallback(&ZoomView::onSizeChanged, this);

	_zoomed.registerForwardSlot(_contentChanged);
	_zoomed.registerForwardSlot(_sizeChanged);
	_zoomed.registerForwardCallback(&ZoomView::onKeyUp, this);
	_zoomed.registerForwardCallback(&ZoomView::onKeyDown, this);
	_zoomed.registerForwardCallback(&ZoomView::onFingerDown, this);
	_zoomed.registerForwardCallback(&ZoomView::onFingerMove, this);

	_zoomed->setAutoscale();
	_zoomed->setDesiredSize(desiredSize);
}

void
ZoomView::updateOutputs() {

	LOG_ALL(zoomviewlog) << "\"updating\" output..." << std::endl;

	_zoomed->updateScaleAndShift();
}

bool
ZoomView::filter(PointerSignal& signal) {

	signal.position = _zoomed->invert(signal.position);
	return true;
}

void
ZoomView::onInputSet(const pipeline::InputSet<Painter>& signal) {

	LOG_ALL(zoomviewlog) << "got a new painter" << std::endl;

	if (!_zoomed)
		_zoomed.createData();

	_zoomed->setContent(_content);

	setDirty(_zoomed);

	_contentChanged();
	_sizeChanged(SizeChanged(_zoomed->getSize()));
}

void
ZoomView::onContentChanged(const ContentChanged& signal) {

	_contentChanged();

	setDirty(_zoomed);
}

void
ZoomView::onSizeChanged(const SizeChanged& signal) {

	_sizeChanged(SizeChanged(_zoomed->getSize()));

	setDirty(_zoomed);
}

void
ZoomView::onResize(const Resize& signal) {

	LOG_ALL(zoomviewlog) << "got a resize signal, change desired size to " << signal.getSize() << std::endl;

	_zoomed->setDesiredSize(signal.getSize());

	_sizeChanged(SizeChanged(signal.getSize()));

	setDirty(_zoomed);
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

		_zoomed->reset();

		setDirty(_zoomed);

		signal.processed = true;

	} else {

		_keyDown(signal);
	}
}

void
ZoomView::onFingerDown(const FingerDown& signal) {

	LOG_ALL(zoomviewlog) << "a finger was put down" << std::endl;

	util::point<double> position = signal.position;

	_fingerDown = position;

	// if shift is pressed, increase zoom speed
	double zoomStep = _zoomStep;
	if (signal.modifiers & keys::ShiftDown)
		zoomStep *= 2;

	//// mouse wheel up
	//if (signal.button == buttons::WheelUp) {

		//LOG_ALL(zoomviewlog) << "it's the left wheel up" << std::endl;

		//_zoomed->zoom(zoomStep, position);
	//}

	//// mouse wheel down
	//if (signal.button == buttons::WheelDown) {

		//LOG_ALL(zoomviewlog) << "it's the left wheel down" << std::endl;

		//_zoomed->zoom(1.0/zoomStep, position);
	//}

	setDirty(_zoomed);
}

void
ZoomView::onFingerMove(const FingerMove& signal) {

	LOG_ALL(zoomviewlog) << "the finger is moved" << std::endl;

	LOG_ALL(zoomviewlog) << "I am in dragging mode" << std::endl;

	double amp = 1.0;
	if (signal.modifiers & keys::ShiftDown)
		amp = 10.0;

	LOG_ALL(zoomviewlog) << "left button is still pressed" << std::endl;

	util::point<double> moved = signal.position - _fingerDown;

	_zoomed->drag(moved*amp);

	_fingerDown = signal.position;

	setDirty(_zoomed);
}

} // namespace gui
