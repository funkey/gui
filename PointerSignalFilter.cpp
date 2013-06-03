#include "PointerSignalFilter.h"

namespace gui {

void
PointerSignalFilter::filterBackward(pipeline::OutputBase& output, pipeline::InputBase& input, pipeline::ProcessNode* processNode) {

	output.registerForwardCallback(boost::function<void(FingerUp&)>(boost::bind(&PointerSignalFilter::onFingerUp,   this, _1)), processNode, signals::Transparent);
	output.registerForwardCallback(boost::function<void(FingerDown&)>(boost::bind(&PointerSignalFilter::onFingerDown, this, _1)), processNode, signals::Transparent);
	output.registerForwardCallback(boost::function<void(FingerMove&)>(boost::bind(&PointerSignalFilter::onFingerMove, this, _1)), processNode, signals::Transparent);
	output.registerForwardCallback(boost::function<void(PenUp&)>(boost::bind(&PointerSignalFilter::onPenUp,   this, _1)), processNode, signals::Transparent);
	output.registerForwardCallback(boost::function<void(PenDown&)>(boost::bind(&PointerSignalFilter::onPenDown, this, _1)), processNode, signals::Transparent);
	output.registerForwardCallback(boost::function<void(PenMove&)>(boost::bind(&PointerSignalFilter::onPenMove, this, _1)), processNode, signals::Transparent);
	output.registerForwardCallback(boost::function<void(MouseUp&)>(boost::bind(&PointerSignalFilter::onMouseUp,   this, _1)), processNode, signals::Transparent);
	output.registerForwardCallback(boost::function<void(MouseDown&)>(boost::bind(&PointerSignalFilter::onMouseDown, this, _1)), processNode, signals::Transparent);
	output.registerForwardCallback(boost::function<void(MouseMove&)>(boost::bind(&PointerSignalFilter::onMouseMove, this, _1)), processNode, signals::Transparent);

	input.registerBackwardSlot(_fingerUp);
	input.registerBackwardSlot(_fingerDown);
	input.registerBackwardSlot(_fingerMove);
	input.registerBackwardSlot(_penUp);
	input.registerBackwardSlot(_penDown);
	input.registerBackwardSlot(_penMove);
	input.registerBackwardSlot(_mouseUp);
	input.registerBackwardSlot(_mouseDown);
	input.registerBackwardSlot(_mouseMove);
}

void
PointerSignalFilter::onFingerUp(FingerUp& signal) {

	filterSignal(signal, _fingerUp);
}

void
PointerSignalFilter::onFingerDown(FingerDown& signal) {

	filterSignal(signal, _fingerDown);
}

void
PointerSignalFilter::onFingerMove(FingerMove& signal) {

	filterSignal(signal, _fingerMove);
}

void
PointerSignalFilter::onPenUp(PenUp& signal) {

	filterSignal(signal, _penUp);
}

void
PointerSignalFilter::onPenDown(PenDown& signal) {

	filterSignal(signal, _penDown);
}

void
PointerSignalFilter::onPenMove(PenMove& signal) {

	filterSignal(signal, _penMove);
}

void
PointerSignalFilter::onMouseUp(MouseUp& signal) {

	filterSignal(signal, _mouseUp);
}

void
PointerSignalFilter::onMouseDown(MouseDown& signal) {

	filterSignal(signal, _mouseDown);
}

void
PointerSignalFilter::onMouseMove(MouseMove& signal) {

	filterSignal(signal, _mouseMove);
}

template <typename SignalType>
void
PointerSignalFilter::filterSignal(SignalType& signal, signals::Slot<SignalType>& slot) {

	if (signal.processed)
		return;
	filter(signal);

	slot(signal);
}

} // namespace gui
