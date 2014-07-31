#include <sstream>
#include <iomanip>
#include <gui/OpenGl.h>
#include "SliderPainter.h"
#include "TextPainter.h"

namespace gui {

SliderPainter::SliderPainter(double min, double max, double value) :
	_value(value),
	_min(min),
	_max(max),
	_highlight(false) {

	setSize(0, 0, 100, 10);

	updateSliderPosition();
}

void
SliderPainter::setValue(double value) {

	_value = value;

	updateSliderPosition();
}

void
SliderPainter::setHighlight(bool highlight) {

	_highlight = highlight;
}

void
SliderPainter::unsetHoverValue() {

	_showHoverValue = false;
}

bool
SliderPainter::draw(
		const util::rect<double>&  roi,
		const util::point<double>& resolution) {

	if (_highlight)
		glColor4f(1.0, 0.75, 0.75, 0.5);
	else
		glColor4f(0.75, 0.75, 0.75, 0.5);

	const util::rect<double>& size = getSize();

	glDisable(GL_DEPTH_TEST);

	glLineWidth(1);
	glDisable(GL_LINE_SMOOTH);

	glBegin(GL_LINES);
	glVertex2d(size.minX, size.minY + size.height()/2);
	glVertex2d(size.maxX, size.minY + size.height()/2);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2d(_graspSize.minX, _graspSize.maxY);
	glVertex2d(_graspSize.maxX, _graspSize.maxY);
	glVertex2d(_graspSize.maxX, _graspSize.minY);
	glVertex2d(_graspSize.minX, _graspSize.minY);
	glEnd();

	if (_showHoverValue) {

		TextPainter hoverValuePainter;


		hoverValuePainter.setText(_hoverText);
		hoverValuePainter.setTextSize(10/resolution.x);

		util::point<double> offset;
		offset.x = _hoverPosition - hoverValuePainter.getSize().width()/2.0;
		offset.y = size.minY + size.height()/2 - 10/resolution.y;

		glBegin(GL_LINES);
		glVertex2d(_hoverPosition, offset.y);
		glVertex2d(_hoverPosition, size.minY + size.height()/2);
		glEnd();

		offset.y -= 10/resolution.y;

		glTranslatef(offset.x, offset.y, 0);
		hoverValuePainter.draw(roi - offset, resolution);
		glTranslatef(-offset.x, -offset.y, 0);
	}

	return false;
}

const util::rect<double>&
SliderPainter::getGraspSize() {

	return _graspSize;
}

void
SliderPainter::updateSliderPosition() {

	const util::rect<double>& size = getSize();

	double sliderWidth = 0.1*size.width();
	
	double sliderPos =
			sliderWidth/2 + (_value - _min)/
			(_max - _min)*(size.width() - sliderWidth);

	_graspSize = util::rect<double>(
			sliderPos - sliderWidth/2,
			size.minY,
			sliderPos + sliderWidth/2,
			size.maxY);
}

} // namespace gui
