#ifndef GUI_POINTER_SIGNAL_FILTER_H__
#define GUI_POINTER_SIGNAL_FILTER_H__

#include "FingerSignals.h"
#include "PenSignals.h"
#include "MouseSignals.h"

namespace gui {

class PointerSignalFilter {

public:

	template <typename OutType, typename InType>
	void filterBackward(OutType& output, InType& input, pipeline::ProcessNode* processNode) {

		output.registerForwardCallback(boost::function<void(FingerUp&)>(boost::bind(&PointerSignalFilter::onFingerUp,   this, _1)), processNode);
		output.registerForwardCallback(boost::function<void(FingerDown&)>(boost::bind(&PointerSignalFilter::onFingerDown, this, _1)), processNode);
		output.registerForwardCallback(boost::function<void(FingerMove&)>(boost::bind(&PointerSignalFilter::onFingerMove, this, _1)), processNode);
		output.registerForwardCallback(boost::function<void(PenUp&)>(boost::bind(&PointerSignalFilter::onPenUp,   this, _1)), processNode);
		output.registerForwardCallback(boost::function<void(PenDown&)>(boost::bind(&PointerSignalFilter::onPenDown, this, _1)), processNode);
		output.registerForwardCallback(boost::function<void(PenMove&)>(boost::bind(&PointerSignalFilter::onPenMove, this, _1)), processNode);
		output.registerForwardCallback(boost::function<void(MouseUp&)>(boost::bind(&PointerSignalFilter::onMouseUp,   this, _1)), processNode);
		output.registerForwardCallback(boost::function<void(MouseDown&)>(boost::bind(&PointerSignalFilter::onMouseDown, this, _1)), processNode);
		output.registerForwardCallback(boost::function<void(MouseMove&)>(boost::bind(&PointerSignalFilter::onMouseMove, this, _1)), processNode);

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

	void onFingerUp(FingerUp& signal) {

		filterSignal(signal, _fingerUp);
	}

	void onFingerDown(FingerDown& signal) {

		filterSignal(signal, _fingerDown);
	}

	void onFingerMove(FingerMove& signal) {

		filterSignal(signal, _fingerMove);
	}

	void onPenUp(PenUp& signal) {

		filterSignal(signal, _penUp);
	}

	void onPenDown(PenDown& signal) {

		filterSignal(signal, _penDown);
	}

	void onPenMove(PenMove& signal) {

		LOG_DEBUG(logger::out) << "[PointerSignalFilter] received pen move signal" << std::endl;
		filterSignal(signal, _penMove);
	}

	void onMouseUp(MouseUp& signal) {

		filterSignal(signal, _mouseUp);
	}

	void onMouseDown(MouseDown& signal) {

		filterSignal(signal, _mouseDown);
	}

	void onMouseMove(MouseMove& signal) {

		filterSignal(signal, _mouseMove);
	}

	template <typename SignalType>
	void filterSignal(SignalType& signal, signals::Slot<SignalType>& slot) {

		LOG_DEBUG(logger::out) << "[PointerSignalFilter] trying to filter signal " << typeName(signal) << " at " << signal.position << std::endl;

		if (signal.processed)
			return;
		LOG_DEBUG(logger::out) << "[PointerSignalFilter] filtering signal " << typeName(signal) << " at " << signal.position << std::endl;
		filter(signal);
		LOG_DEBUG(logger::out) << "[PointerSignalFilter] filtered, signal is now " << typeName(signal) << " at " << signal.position << std::endl;

		slot(signal);
	}

	signals::Slot<FingerUp>   _fingerUp;
	signals::Slot<FingerDown> _fingerDown;
	signals::Slot<FingerMove> _fingerMove;
	signals::Slot<PenUp>      _penUp;
	signals::Slot<PenDown>    _penDown;
	signals::Slot<PenMove>    _penMove;
	signals::Slot<MouseUp>    _mouseUp;
	signals::Slot<MouseDown>  _mouseDown;
	signals::Slot<MouseMove>  _mouseMove;
};

};

#endif // GUI_POINTER_SIGNAL_FILTER_H__

