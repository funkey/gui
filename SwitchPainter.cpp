#include <gui/OpenGl.h>
#include "SwitchPainter.h"

namespace gui {

SwitchPainter::SwitchPainter(bool value) :
	_value(value),
	_highlight(false) {

	setSize(0, 0, 10, 10);
}

void
SwitchPainter::setValue(bool value) {

	_value = value;
}

bool
SwitchPainter::getValue() {

	return _value;
}

void
SwitchPainter::setHighlight(bool highlight) {

	_highlight = highlight;
}

void
SwitchPainter::draw(
		const util::rect<double>&  roi,
		const util::point<double>& resolution) {

	double r, g, b, a;

	if (_value) {

		r = 0.1;
		g = 0.5;
		b = 0.0;
		a = 0.5;

	} else {

		r = 0.5;
		g = 0.1;
		b = 0.0;
		a = 0.5;
	}

	if (_highlight) {

		r *= 2.0;
		g *= 2.0;
		b *= 2.0;
	}

	const util::rect<double>& size = getSize();

	glColor4f(r, g, b, a);

	glBegin(GL_QUADS);
	glVertex2d(size.minX, size.minY);
	glVertex2d(size.maxX, size.minY);
	glVertex2d(size.maxX, size.maxY);
	glVertex2d(size.minX, size.maxY);
	glEnd();
}

} // namespace gui
