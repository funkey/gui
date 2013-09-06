#include <util/Logger.h>
#include "SwitchImpl.h"

using namespace logger;

LogChannel switchlog("switchlog", "");

namespace gui {

SwitchImpl::SwitchImpl(bool value) :
	_value(value),
	_painter(new SwitchPainter(value)),
	_mouseOver(false) {

	registerOutput(_value, "value");
	registerOutput(_painter, "painter");

	_painter.registerForwardCallback(&SwitchImpl::onMouseUp, this);
	_painter.registerForwardCallback(&SwitchImpl::onMouseMove, this);
}

void
SwitchImpl::updateOutputs() {

	*_value = _painter->getValue();
}

void
SwitchImpl::onMouseUp(MouseUp& signal) {

	if (signal.button == buttons::Left) {

		const util::rect<double>& size = _painter->getSize();

		if (size.contains(signal.position)) {

			LOG_ALL(switchlog) << "left button up" << std::endl;

			*_value = !(*_value);

			_painter->setValue(*_value);

			setDirty(_painter);

			setDirty(_value);
		}
	}
}

void
SwitchImpl::onMouseMove(MouseMove& signal) {

	LOG_ALL(switchlog) << "mouse moved at " << signal.position << std::endl;

	const util::rect<double>& size = _painter->getSize();

	if (size.contains(signal.position)) {

		LOG_ALL(switchlog) << "...inside switch" << std::endl;

		if (_mouseOver == false) {

			_mouseOver = true;

			_painter->setHighlight(true);

			setDirty(_painter);
		}

	} else {

		LOG_ALL(switchlog) << "...outside switch" << std::endl;

		if (_mouseOver == true) {

			_mouseOver = false;

			_painter->setHighlight(false);

			setDirty(_painter);
		}
	}
}

} // namespace gui

