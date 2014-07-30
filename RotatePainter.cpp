#include <gui/OpenGl.h>
#include <util/Logger.h>
#include "RotatePainter.h"

namespace gui {

logger::LogChannel rotatepainterlog("rotatepainterlog", "[RotatePainter] ");

RotatePainter::RotatePainter() :
	_highlight(false) {}

void
RotatePainter::setContent(boost::shared_ptr<Painter> content) {

	if (!content)
		return;

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

bool
RotatePainter::draw(const util::rect<double>& roi, const util::point<double>& resolution) {

	if (!_content)
		return false;

	OpenGl::Guard guard;

	glMatrixMode(GL_MODELVIEW);

	glPushMatrix();

	// set the light position (before rotating), but don't turn it on, yet
	GLfloat lightpos[] = {-1.0, 1.0, -1.0, 0.0};
	glLightfv(GL_LIGHT1, GL_POSITION, lightpos);
	glDisable(GL_LIGHTING);

	glTranslated( _centerX,  _centerY,  _centerZ);
	glRotated(_w, _x, _y, _z);
	glTranslated(-_centerX, -_centerY, -_centerZ);

	util::point<double> ul = _content->getSize().upperLeft();
	util::point<double> lr = _content->getSize().lowerRight();

	glColor4f((_highlight ? 0.88 : 0.1), 0.2, 0.05, 0.5);

	// draw 2d frame around content
	glCheck(glEnable(GL_BLEND));
	glCheck(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	glLineWidth(2.0);
	glEnable(GL_LINE_SMOOTH);

	glBegin(GL_LINES);
	glVertex2f(ul.x, ul.y);
	glVertex2f(lr.x, ul.y);

	glVertex2f(lr.x, ul.y);
	glVertex2f(lr.x, lr.y);

	glVertex2f(lr.x, lr.y);
	glVertex2f(ul.x, lr.y);

	glVertex2f(ul.x, lr.y);
	glVertex2f(ul.x, ul.y);
	glEnd();

	// draw solid backside
	glCheck(glEnable(GL_CULL_FACE));
	glCheck(glEnable(GL_LIGHTING));
	glCheck(glEnable(GL_LIGHT0));
	glCheck(glEnable(GL_COLOR_MATERIAL));

	glBegin(GL_QUADS);
	glVertex2f(ul.x, ul.y);
	glVertex2f(lr.x, ul.y);
	glVertex2f(lr.x, lr.y);
	glVertex2f(ul.x, lr.y);
	glEnd();

	// turn on the light
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT1);
	GLfloat ambient[] = {0.5, 0.5, 0.5, 1.0};
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	GLfloat specular[] = {0.3, 0.3, 0.3, 1.0};
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	GLfloat diffuse[] = {0.5, 0.5, 0.5, 1.0};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);

	// draw content
	bool wantsRedraw = _content->draw(roi, resolution);

	glPopMatrix();

	return wantsRedraw;
}

void
RotatePainter::updateSize() {

	if (!_content)
		return;

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

