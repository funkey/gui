#include "PointerSignalFilter.h"

namespace gui {

#define POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK(s) \
	output.registerForwardCallback(boost::function<void(s&)>(boost::bind(&PointerSignalFilter::on##s, this, _1)), processNode, signals::Transparent);

void
PointerSignalFilter::filterBackward(pipeline::OutputBase& output, pipeline::InputBase& input, pipeline::ProcessNode* processNode) {

	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK(FingerUp)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK(FingerDown)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK(FingerMove)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK(PenUp)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK(PenDown)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK(PenMove)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK(MouseUp)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK(MouseDown)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK(MouseMove)

	input.registerBackwardSlot(_slotFingerUp);
	input.registerBackwardSlot(_slotFingerDown);
	input.registerBackwardSlot(_slotFingerMove);
	input.registerBackwardSlot(_slotPenUp);
	input.registerBackwardSlot(_slotPenDown);
	input.registerBackwardSlot(_slotPenMove);
	input.registerBackwardSlot(_slotPenIn);
	input.registerBackwardSlot(_slotPenOut);
	input.registerBackwardSlot(_slotPenAway);
	input.registerBackwardSlot(_slotMouseUp);
	input.registerBackwardSlot(_slotMouseDown);
	input.registerBackwardSlot(_slotMouseMove);
}

#define POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK_MULTI(s) \
	output.registerForwardCallback(boost::function<void(s&)>(boost::bind(&PointerSignalFilter::onMulti##s, this, _1)), processNode, signals::Transparent);

void
PointerSignalFilter::filterBackward(pipeline::OutputBase& output, pipeline::MultiInput& inputs, pipeline::ProcessNode* processNode) {

	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK_MULTI(FingerUp)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK_MULTI(FingerDown)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK_MULTI(FingerMove)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK_MULTI(PenUp)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK_MULTI(PenDown)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK_MULTI(PenMove)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK_MULTI(MouseUp)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK_MULTI(MouseDown)
	POINTER_SIGNAL_FILTER_REGISTER_FORWARD_CALLBACK_MULTI(MouseMove)

	inputs.registerBackwardSlots(_slotsFingerUp);
	inputs.registerBackwardSlots(_slotsFingerDown);
	inputs.registerBackwardSlots(_slotsFingerMove);
	inputs.registerBackwardSlots(_slotsPenUp);
	inputs.registerBackwardSlots(_slotsPenDown);
	inputs.registerBackwardSlots(_slotsPenMove);
	inputs.registerBackwardSlots(_slotsPenIn);
	inputs.registerBackwardSlots(_slotsPenOut);
	inputs.registerBackwardSlots(_slotsPenAway);
	inputs.registerBackwardSlots(_slotsMouseUp);
	inputs.registerBackwardSlots(_slotsMouseDown);
	inputs.registerBackwardSlots(_slotsMouseMove);
}

#define POINTER_SIGNAL_FILTER_ON(s) \
	void \
	PointerSignalFilter::on##s(s& signal) { \
	\
		filterSignal(signal, _slot##s); \
	}

POINTER_SIGNAL_FILTER_ON(FingerUp)
POINTER_SIGNAL_FILTER_ON(FingerDown)
POINTER_SIGNAL_FILTER_ON(FingerMove)
POINTER_SIGNAL_FILTER_ON(PenUp)
POINTER_SIGNAL_FILTER_ON(PenDown)
POINTER_SIGNAL_FILTER_ON(PenMove)
POINTER_SIGNAL_FILTER_ON(MouseUp)
POINTER_SIGNAL_FILTER_ON(MouseDown)
POINTER_SIGNAL_FILTER_ON(MouseMove)

#define POINTER_SIGNAL_FILTER_ON_MULTI(s) \
	void \
	PointerSignalFilter::onMulti##s(s& signal) { \
	\
		for (unsigned int i = 0; i < _slots##s.size(); i++) \
			filterSignal(signal, i, _slots##s); \
	}

POINTER_SIGNAL_FILTER_ON_MULTI(FingerUp)
POINTER_SIGNAL_FILTER_ON_MULTI(FingerDown)
POINTER_SIGNAL_FILTER_ON_MULTI(FingerMove)
POINTER_SIGNAL_FILTER_ON_MULTI(PenUp)
POINTER_SIGNAL_FILTER_ON_MULTI(PenDown)
POINTER_SIGNAL_FILTER_ON_MULTI(PenMove)
POINTER_SIGNAL_FILTER_ON_MULTI(MouseUp)
POINTER_SIGNAL_FILTER_ON_MULTI(MouseDown)
POINTER_SIGNAL_FILTER_ON_MULTI(MouseMove)

template <typename SignalType>
void
PointerSignalFilter::filterSignal(SignalType& signal, signals::Slot<SignalType>& slot) {

	if (signal.processed)
		return;
	filter(signal);

	slot(signal);
}

template <typename SignalType>
void
PointerSignalFilter::filterSignal(SignalType& signal, unsigned int i, signals::Slots<SignalType>& slots) {

	if (signal.processed)
		return;
	filter(signal, i);

	slots[i](signal);
}

} // namespace gui
