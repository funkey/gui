#ifndef GUI_POINTER_SIGNAL_FILTER_H__
#define GUI_POINTER_SIGNAL_FILTER_H__

#include <pipeline/all.h>

#include "FingerSignals.h"
#include "PenSignals.h"
#include "MouseSignals.h"

namespace gui {

class PointerSignalFilter {

public:

	void filterBackward(pipeline::OutputBase& output, pipeline::InputBase& input, pipeline::ProcessNode* processNode);

	void filterBackward(pipeline::OutputBase& output, pipeline::MultiInput& inputs, pipeline::ProcessNode* processNode);

protected:

	/**
	 * The filter method, to be implemented by subclasses.
	 *
	 * @return
	 *         Implementations should return false, if the given signal should 
	 *         not be forwarded.
	 */
	virtual bool filter(PointerSignal& signal) = 0;

	/**
	 * Filter method for one-to-many filters (from one output to several 
	 * inputs). Will be called with the appropriate input number.
	 */
	virtual bool filter(PointerSignal& signal, unsigned int input) { return true; }

private:

	void onFingerUp(FingerUp& signal);
	void onFingerDown(FingerDown& signal);
	void onFingerMove(FingerMove& signal);
	void onPenUp(PenUp& signal);
	void onPenDown(PenDown& signal);
	void onPenMove(PenMove& signal);
	void onPenIn(PenIn& signal);
	void onPenOut(PenOut& signal);
	void onPenAway(PenAway& signal);
	void onMouseUp(MouseUp& signal);
	void onMouseDown(MouseDown& signal);
	void onMouseMove(MouseMove& signal);

	void onMultiFingerUp(FingerUp& signal);
	void onMultiFingerDown(FingerDown& signal);
	void onMultiFingerMove(FingerMove& signal);
	void onMultiPenUp(PenUp& signal);
	void onMultiPenDown(PenDown& signal);
	void onMultiPenMove(PenMove& signal);
	void onMultiPenIn(PenIn& signal);
	void onMultiPenOut(PenOut& signal);
	void onMultiPenAway(PenAway& signal);
	void onMultiMouseUp(MouseUp& signal);
	void onMultiMouseDown(MouseDown& signal);
	void onMultiMouseMove(MouseMove& signal);

	template <typename SignalType>
	void filterSignal(SignalType& signal, signals::Slot<SignalType>& slot);

	template <typename SignalType>
	void filterSignal(SignalType& signal, unsigned int i, signals::Slots<SignalType>& slots);

	signals::Slot<FingerUp>   _slotFingerUp;
	signals::Slot<FingerDown> _slotFingerDown;
	signals::Slot<FingerMove> _slotFingerMove;
	signals::Slot<PenUp>      _slotPenUp;
	signals::Slot<PenDown>    _slotPenDown;
	signals::Slot<PenMove>    _slotPenMove;
	signals::Slot<PenIn>      _slotPenIn;
	signals::Slot<PenOut>     _slotPenOut;
	signals::Slot<PenAway>    _slotPenAway;
	signals::Slot<MouseUp>    _slotMouseUp;
	signals::Slot<MouseDown>  _slotMouseDown;
	signals::Slot<MouseMove>  _slotMouseMove;

	signals::Slots<FingerUp>   _slotsFingerUp;
	signals::Slots<FingerDown> _slotsFingerDown;
	signals::Slots<FingerMove> _slotsFingerMove;
	signals::Slots<PenUp>      _slotsPenUp;
	signals::Slots<PenDown>    _slotsPenDown;
	signals::Slots<PenMove>    _slotsPenMove;
	signals::Slots<PenIn>      _slotsPenIn;
	signals::Slots<PenOut>     _slotsPenOut;
	signals::Slots<PenAway>    _slotsPenAway;
	signals::Slots<MouseUp>    _slotsMouseUp;
	signals::Slots<MouseDown>  _slotsMouseDown;
	signals::Slots<MouseMove>  _slotsMouseMove;
};

};

#endif // GUI_POINTER_SIGNAL_FILTER_H__

