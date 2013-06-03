#ifndef GUI_POINTER_SIGNAL_H__
#define GUI_POINTER_SIGNAL_H__

#include "GuiSignals.h"

namespace gui {

/**
 * Base class for all 2D input signals (mouse, pen, touch, ...).
 */
class PointerSignal : public InputSignal {

public:

	PointerSignal() :
		position(0, 0) {}

	PointerSignal(const util::point<double>& position_) :
		position(position_) {}

	util::point<double> position;
};

};

#endif // GUI_POINTER_SIGNAL_H__

