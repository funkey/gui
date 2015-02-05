#include <util/Logger.h>
#include <gui/OpenGl.h>
#include "ZoomPainter.h"

namespace gui {

logger::LogChannel zoompainterlog("zoompainterlog", "[ZoomPainter] ");

ZoomPainter::ZoomPainter() :
	_userScale(1.0),
	_userShift(0, 0),
	_autoScale(1.0),
	_autoShift(0, 0),
	_scale(1.0),
	_shift(0, 0),
	_autoscale(false),
	_desiredSize(0, 0, 1, 1),
	_z2d(1000),
	_zClipNear(1),
	_zClipFar(2000) {}

void
ZoomPainter::setContent(boost::shared_ptr<Painter> content) {

	_content = content;

	if (_autoscale)

		// we rescale to the desired size
		setSize(_desiredSize);
	
	else
		// the reported size should be the size of the original content
		setSize(_content->getSize());

	updateScaleAndShift();
}

void
ZoomPainter::setUserScale(double scale) {

	_userScale = scale;

	updateScaleAndShift();
}

void
ZoomPainter::setUserShift(const util::point<double>& shift) {

	_userShift = shift;

	updateScaleAndShift();
}

void
ZoomPainter::reset() {

	_userScale = 1.0;
	_userShift = util::point<double>(0.0, 0.0);

	updateScaleAndShift();
}

util::point<double>
ZoomPainter::invert(const util::point<double>& point) {

	util::point<double> inv = point;

	inv -= _shift;
	inv /= _scale;

	return inv;
}

void
ZoomPainter::zoom(double zoomChange, const util::point<double>& anchor) {

	LOG_ALL(zoompainterlog) << "changing user zoom by " << zoomChange << " keeping " << anchor << " where it is" << std::endl;

	_userScale *= zoomChange;
	_userShift  = (anchor - zoomChange*(anchor - (_autoScale*_userShift + _autoShift)) - _autoShift)/_autoScale;

	updateScaleAndShift();
}

void
ZoomPainter::drag(const util::point<double>& direction) {

	_userShift += direction/_autoScale;

	updateScaleAndShift();
}

bool
ZoomPainter::draw(const util::rect<double>& roi, const util::point<double>& resolution) {

	if (!_content)
		return false;

	LOG_ALL(zoompainterlog) << "drawing" << std::endl;

	LOG_ALL(zoompainterlog) << "shift is " << _shift << ", scale is " << _scale << std::endl;

	OpenGl::Guard guard;

	/* Here, we configre the gl frustum to match the zoomed roi size. In the 
	 * middle of the frustum, at z = _z2d, we have a plane where the x and y 
	 * coordinates match the pixels of the roi, such that (x, y) = (0, 0) is the 
	 * upper left pixel, if _shift is (0, 0).
	 *
	 * Further, we change the gl coordinate system, such that we obtain a 
	 * right-handed coordinate system where the positive directions of x, y, and 
	 * z point right, down, and back, respectively.
	 */

	util::rect<double> zoomedRoi = (roi - _shift)/_scale;

	/* To obtain proper perspective deformations, we set the frustum such that 
	 * the vanishing point is in the middle of the zoomed roi. During drawing, 
	 * we change the modelview matrix to compensate for that, such that zoomed 
	 * roi upper left is in the frustum upper left.
	 */

	/* Next, we simulate the zoom by modifying the frustum: Instead of scaling 
	 * the world coordinates, we inversely scale the frustum by using the width 
	 * and height of the zoomed roi.
	 */
	double l2d = - zoomedRoi.width()/2;
	double r2d = l2d + zoomedRoi.width();
	double t2d = - zoomedRoi.height()/2;
	double b2d = t2d + zoomedRoi.height();

	double z2d   = _z2d/_scale;
	double zNear = _zClipNear/_scale;
	double zFar  = _zClipFar/_scale;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glFrustum(
			l2d*zNear/z2d, // left
			r2d*zNear/z2d, // right
			b2d*zNear/z2d, // bottom
			t2d*zNear/z2d, // top
			zNear,         // near
			zFar           // far
	);

	/* It remains to invert the z-axis to obtain the coordinate system as 
	 * described above. This is done here, together with translating the world 
	 * coordinates z=0 to z=_z2d and compensating for the upper left of the 
	 * zoomed roi.
	 */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-zoomedRoi.minX - zoomedRoi.width()/2, -zoomedRoi.minY - zoomedRoi.height()/2, -z2d);
	glScalef(1, 1, -1);

	bool wantsRedraw = _content->draw(zoomedRoi, resolution*_scale);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	return wantsRedraw;
}

void
ZoomPainter::updateScaleAndShift() {

	_autoScale = 1.0;
	_autoShift = util::point<double>(0, 0);

	// first, apply autoscale transformation (if wanted)
	if (_autoscale && _content) {

		const util::rect<double>& contentSize = _content->getSize();

		// do we have to fit the width or height of the content?
		bool fitHeight = (contentSize.width()/contentSize.height() < _desiredSize.width()/_desiredSize.height());

		// get the scale to fit the width or height to the desired size
		_autoScale = (fitHeight ? _desiredSize.height()/contentSize.height() : _desiredSize.width()/contentSize.width());

		// get the shift to center the content in the desired area relative to 
		// desired upper left
		util::point<double> centerShift =
				(fitHeight ?
				 util::point<double>(1, 0)*0.5*(_desiredSize.width()  - contentSize.width() *_autoScale) :
				 util::point<double>(0, 1)*0.5*(_desiredSize.height() - contentSize.height()*_autoScale));

		// get the final shift relative to content upper left
		_autoShift = (contentSize.upperLeft() - _desiredSize.upperLeft()) + centerShift;
	}

	// append user scale and shift transformation
	_shift = _autoScale*_userShift + _autoShift;
	_scale = _autoScale*_userScale;
}

} // namespace gui
