#include <util/Logger.h>
#include "SwitchImpl.h"

using namespace logger;

LogChannel switchlog("switchlog", "");

namespace gui {

SwitchImpl::SwitchImpl() :
	_value(false),
	_painter(new SwitchPainter(false)),
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

	const util::rect<double>& size = _painter->getSize();

	if (size.contains(signal.position)) {

		if (_mouseOver == false) {

			_mouseOver = true;

			_painter->setHighlight(true);

			setDirty(_painter);
		}

	} else {

		if (_mouseOver == true) {

			_mouseOver = false;

			_painter->setHighlight(false);

			setDirty(_painter);
		}
	}
}

} // namespace gui

