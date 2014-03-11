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
	_desiredSize(0, 0, 1, 1) {}

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

	glPushMatrix();

	glTranslated(_shift.x, _shift.y, 0.0);
	glScaled(_scale, _scale, 1.0);

	bool wantsRedraw = _content->draw((roi - _shift)/_scale, resolution*_scale);

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
