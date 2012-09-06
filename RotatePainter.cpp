#include <gui/OpenGl.h>
#include <util/Logger.h>
#include "RotatePainter.h"

namespace gui {

logger::LogChannel rotatepainterlog("rotatepainterlog", "[RotatePainter] ");

void
RotatePainter::setContent(boost::shared_ptr<Painter> content) {

	_content = content;

	updateSize();
}

void
RotatePainter::setRotation(double x, double y, double z, double w) {

	_x = x;
	_y = y;
	_z = z;
	_w = w;
}

void
RotatePainter::draw(const util::rect<double>& roi, const util::point<double>& resolution) {

	OpenGl::Guard guard;

	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();

	glTranslated( _centerX,  _centerY,  _centerZ);
	glRotated(_w, _x, _y, _z);
	glTranslated(-_centerX, -_centerY, -_centerZ);

	_content->draw(roi + util::point<double>(_centerX, _centerY), resolution);

	glPopMatrix();
}

void
RotatePainter::updateSize() {

	util::rect<double> size = _content->getSize();

	LOG_ALL(rotatepainterlog) << "content size is " << size << std::endl;

	_centerX = 0.5*(_content->getSize().minX + _content->getSize().maxX);
	_centerY = 0.5*(_content->getSize().minY + _content->getSize().maxY);
	_centerZ = 0;

	LOG_ALL(rotatepainterlog) << "center is at " << _centerX << ", " << _centerY << std::endl;

	// the reported size should be the size of the original content
	setSize(size);
}

} // namespace gui

