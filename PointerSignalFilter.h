#ifndef GUI_POINTER_SIGNAL_FILTER_H__
#define GUI_POINTER_SIGNAL_FILTER_H__

#include <pipeline/all.h>
#include <pipeline/SignalFilter.h>

#include "FingerSignals.h"
#include "PenSignals.h"
#include "MouseSignals.h"

namespace gui {

class PointerSignalFilter :
		public pipeline::SignalFilter<
				pipeline::FilterSignal<MouseUp,
				pipeline::FilterSignal<MouseDown,
				pipeline::FilterSignal<MouseMove,
				pipeline::FilterSignalsAs<PointerSignal> > > > > {};

} // namespace gui

#endif // GUI_POINTER_SIGNAL_FILTER_H__

