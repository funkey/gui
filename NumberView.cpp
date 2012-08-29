#include <sstream>
#include <iomanip>

#include "NumberView.h"

namespace gui {

NumberView::NumberView(int precision) :
	_dirty(false),
	_precision(precision) {

	registerInput(_value, "value");
	registerOutput(_painter, "painter");

	_value.registerBackwardCallback(&NumberView::onModified, this);
	_painter.registerForwardCallback(&NumberView::onUpdate, this);
	_painter.registerForwardSlot(_modified);
	_painter.registerForwardSlot(_sizeChanged);
}

void
NumberView::onModified(const pipeline::Modified& signal) {

	_dirty = true;
	_modified();
}

void
NumberView::onUpdate(const pipeline::Update& signal) {

	if (_dirty) {

		std::ostringstream ss;
		ss << std::fixed << std::setprecision(_precision) << *_value;

		_painter->setText(ss.str());
		_dirty = false;

		_sizeChanged();
	}
}

} // namespace gui
