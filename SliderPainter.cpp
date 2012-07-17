#include <gui/OpenGl.h>
#include "SliderPainter.h"

namespace gui {

SliderPainter::SliderPainter(double min, double max, double value) :
	_min(min),
	_max(max),
	_value(value),
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
SliderPainter::draw(
		const util::rect<double>&  roi,
		const util::point<double>& resolution) {

	if (_highlight)
		glColor4f(1.0, 0.75, 0.75, 0.5);
	else
		glColor4f(0.75, 0.75, 0.75, 0.5);

	const util::rect<double>& size = getSize();

	glLineWidth(1);
	glDisable(GL_LINE_SMOOTH);

	glBegin(GL_LINES);
	glVertex2d(size.minX + size.height()/2, size.minY + size.height()/2);
	glVertex2d(size.maxX - size.height()/2, size.minY + size.height()/2);
	glEnd();

	glBegin(GL_QUADS);
	glVertex2d(_graspSize.minX, _graspSize.minY);
	glVertex2d(_graspSize.maxX, _graspSize.minY);
	glVertex2d(_graspSize.maxX, _graspSize.maxY);
	glVertex2d(_graspSize.minX, _graspSize.maxY);
	glEnd();
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
