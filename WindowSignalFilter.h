#ifndef GUI_WINDOW_SIGNAL_FILTER_H__
#define GUI_WINDOW_SIGNAL_FILTER_H__

#include <pipeline/all.h>

#include "WindowSignals.h"

namespace gui {

class WindowSignalFilter {

public:

	void filterForward(pipeline::InputBase& input, pipeline::OutputBase& output, pipeline::ProcessNode* processNode);

protected:

	/**
	 * The filter method, to be overwritten by subclasses.
	 *
	 * @return
	 *         Implementations should return false, if the given signal should 
	 *         not be forwarded.
	 */
	virtual bool filter(WindowSignal& /*signal*/) { return true; }

private:

	void onWindowFullscreen(WindowFullscreen& signal);

	void onWindowIconfiy(WindowIconify& signal);

	template <typename SignalType>
	void filterSignal(SignalType& signal, signals::Slot<SignalType>& slot);

	signals::Slot<WindowFullscreen> _fullscreen;
	signals::Slot<WindowIconify>    _iconify;
};

};


#endif // GUI_WINDOW_SIGNAL_FILTER_H__

