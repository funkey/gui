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

protected:

	/**
	 * The filter method, to be implemented by subclasses.
	 *
	 * @return
	 *         Implementations should return false, if the given signal should 
	 *         not be forwarded.
	 */
	virtual bool filter(PointerSignal& signal) = 0;

private:

	void onFingerUp(FingerUp& signal);

	void onFingerDown(FingerDown& signal);

	void onFingerMove(FingerMove& signal);

	void onPenUp(PenUp& signal);

	void onPenDown(PenDown& signal);

	void onPenMove(PenMove& signal);

	void onPenIn(PenIn& signal);

	void onPenOut(PenOut& signal);

	void onMouseUp(MouseUp& signal);

	void onMouseDown(MouseDown& signal);

	void onMouseMove(MouseMove& signal);

	template <typename SignalType>
	void filterSignal(SignalType& signal, signals::Slot<SignalType>& slot);

	signals::Slot<FingerUp>   _fingerUp;
	signals::Slot<FingerDown> _fingerDown;
	signals::Slot<FingerMove> _fingerMove;
	signals::Slot<PenUp>      _penUp;
	signals::Slot<PenDown>    _penDown;
	signals::Slot<PenMove>    _penMove;
	signals::Slot<PenIn>      _penIn;
	signals::Slot<PenOut>     _penOut;
	signals::Slot<MouseUp>    _mouseUp;
	signals::Slot<MouseDown>  _mouseDown;
	signals::Slot<MouseMove>  _mouseMove;
};

};

#endif // GUI_POINTER_SIGNAL_FILTER_H__

