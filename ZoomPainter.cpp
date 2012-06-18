#include <gui/OpenGl.h>
#include "ZoomPainter.h"

namespace gui {

void
ZoomPainter::setContent(boost::shared_ptr<Painter> content) {

	_content = content;

	updateSize();
}

void
ZoomPainter::setScale(double scale) {

	_scale = scale;
}

void
ZoomPainter::setShift(const util::point<double>& shift) {

	_shift = shift;
}

void
ZoomPainter::draw(const util::rect<double>& roi, const util::point<double>& resolution) {

	OpenGl::Guard guard;

	glPushMatrix();

	glScaled(_scale, _scale, 1.0);
	glTranslated(_shift.x, _shift.y, 0.0);

	_content->draw(roi/_scale - _shift, resolution*_scale);

	glPopMatrix();
}

void
ZoomPainter::updateSize() {

	// the reported size should be the size of the original content
	setSize(_content->getSize());
}

} // namespace gui
