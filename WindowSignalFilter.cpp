#include "WindowSignalFilter.h"

namespace gui {

void
WindowSignalFilter::filterForward(pipeline::InputBase& input, pipeline::OutputBase& output, pipeline::ProcessNode* processNode) {

	input.registerCallback(boost::function<void(WindowFullscreen&)>(boost::bind(&WindowSignalFilter::onWindowFullscreen,   this, _1)), processNode, signals::Transparent);
	input.registerCallback(boost::function<void(WindowIconify&)>(boost::bind(&WindowSignalFilter::onWindowIconfiy,   this, _1)), processNode, signals::Transparent);

	output.registerSlot(_fullscreen);
	output.registerSlot(_iconify);
}

void
WindowSignalFilter::onWindowFullscreen(WindowFullscreen& signal) {

	filterSignal(signal, _fullscreen);
}

void
WindowSignalFilter::onWindowIconfiy(WindowIconify& signal) {

	filterSignal(signal, _iconify);
}

template <typename SignalType>
void
WindowSignalFilter::filterSignal(SignalType& signal, signals::Slot<SignalType>& slot) {

	filter(signal);

	slot(signal);
}

} // namespace gui
