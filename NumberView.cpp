#include <sstream>
#include <iomanip>

#include "NumberView.h"

namespace gui {

NumberView::NumberView(int precision) :
	_precision(precision) {

	registerInput(_value, "value");
	registerOutput(_painter, "painter");

	_painter.registerForwardSlot(_sizeChanged);
}

void
NumberView::updateOutputs() {

	std::ostringstream ss;
	ss << std::fixed << std::setprecision(_precision) << *_value;

	_painter->setText(ss.str());

	_sizeChanged();
}

} // namespace gui
