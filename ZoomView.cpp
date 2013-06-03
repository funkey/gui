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

	// establish pointer signal filter
	PointerSignalFilter::filterBackward(_zoomed, _content, this);

	_content.registerBackwardSlot(_keyDown);
	_content.registerBackwardSlot(_keyUp);
	_content.registerBackwardCallback(&ZoomView::onInputSet, this);
	_content.registerBackwardCallback(&ZoomView::onContentChanged, this);
	_content.registerBackwardCallback(&ZoomView::onSizeChanged, this);

	_zoomed.registerForwardSlot(_contentChanged);
	_zoomed.registerForwardSlot(_sizeChanged);
	_zoomed.registerForwardCallback(&ZoomView::onKeyUp, this);
	_zoomed.registerForwardCallback(&ZoomView::onKeyDown, this);
	_zoomed.registerForwardCallback(&ZoomView::onPenMove, this);
	_zoomed.registerForwardCallback(&ZoomView::onPenIn, this);
	_zoomed.registerForwardCallback(&ZoomView::onPenOut, this);
	_zoomed.registerForwardCallback(&ZoomView::onFingerDown, this);
	_zoomed.registerForwardCallback(&ZoomView::onFingerMove, this);
	_zoomed.registerForwardCallback(&ZoomView::onFingerUp, this);

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

	util::rect<double> oldSize = _zoomed->getSize();

	_zoomed->setDesiredSize(signal.getSize());

	util::rect<double> newSize = _zoomed->getSize();

	if (oldSize != newSize) {

		_sizeChanged(newSize);
		setDirty(_zoomed);
	}
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
ZoomView::onPenMove(const PenMove& signal) {

	_lastPen = signal.position;
}

void
ZoomView::onPenIn(const PenIn& signal) {

	_penClose = true;
}

void
ZoomView::onPenOut(const PenOut& signal) {

	_penClose = false;
}

void
ZoomView::onFingerDown(const FingerDown& signal) {

	LOG_ALL(zoomviewlog) << "a finger was put down (" << _fingerDown.size() << " fingers now)" << std::endl;

	util::point<double> position = signal.position;

	_fingerDown[signal.id] = signal;

	setDirty(_zoomed);
}

void
ZoomView::onFingerMove(const FingerMove& signal) {

	LOG_ALL(zoomviewlog) << "a finger is moved" << std::endl;

	if (_fingerDown.size() > 2)
		return;

	LOG_ALL(zoomviewlog) << "I am in zooming mode (number of fingers down == 2)" << std::endl;

	// get the moving finger
	std::map<int, FingerSignal>::iterator i = _fingerDown.find(signal.id);
	if (i == _fingerDown.end()) {

		LOG_ERROR(zoomviewlog) << "got a move from unseen finger " << signal.id << std::endl;
		return;
	}

	// get the previous position of the finger
	util::point<double>& previousPosition = i->second.position;

	// determine drag, let each finger contribute with equal weight
	util::point<double> moved = (1.0/_fingerDown.size())*(signal.position - previousPosition);

	// if two fingers are down, perform a zoom
	double previousDistance = 0;
	if (_fingerDown.size() == 2) {

		// the previous distance between the fingers
		previousDistance = getFingerDistance();
	}

	// update remembered position
	previousPosition = signal.position;

	// is there a reason not to accept the drag and zoom?
	if (locked(signal.timestamp, signal.position))
		return;

	// set drag
	_zoomed->drag(moved);

	// set zoom
	if (_fingerDown.size() == 2) {

		double distance = getFingerDistance();

		_zoomed->zoom(distance/previousDistance, getFingerCenter());
	}

	setDirty(_zoomed);
}

void
ZoomView::onFingerUp(const FingerUp& signal) {

	std::map<int, FingerSignal>::iterator i = _fingerDown.find(signal.id);
	if (i != _fingerDown.end())
		_fingerDown.erase(i);
}

double
ZoomView::getFingerDistance() {

	if (_fingerDown.size() != 2)
		return 0;

	std::map<int, FingerSignal>::const_iterator i = _fingerDown.begin();

	util::point<double> diff = i->second.position;
	i++;
	diff -= i->second.position;

	return sqrt(diff.x*diff.x + diff.y*diff.y);
}

util::point<double>
ZoomView::getFingerCenter() {

	util::point<double> center;
	for (std::map<int, FingerSignal>::const_iterator i = _fingerDown.begin(); i != _fingerDown.end(); i++)
		center += i->second.position;

	return center/_fingerDown.size();
}

bool
ZoomView::locked(unsigned long now, const util::point<double>& position) {

	// if there is a pen, allow only dragging and moving from an area that is 
	// clearly not the palm (of a right-handed person)
	if (_penClose) {

		if (position.x < _lastPen.x)
			return false;
		else
			return true;
	}
}

} // namespace gui
