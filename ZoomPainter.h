#ifndef GUI_ZOOM_PAINTER_H__
#define GUI_ZOOM_PAINTER_H__

#include <boost/shared_ptr.hpp>

#include <gui/Painter.h>
#include <util/point.hpp>
#include <util/rect.hpp>

namespace gui {

class ZoomPainter : public Painter {

public:

	void setContent(boost::shared_ptr<Painter> content);

	void setScale(double scale);

	void setShift(const util::point<double>& shift);

	void draw(const util::rect<double>& roi, const util::point<double>& resolution);

	void updateSize();

private:

	// the scale of this painter
	double        _scale;

	// the shift of this painter
	util::point<double> _shift;

	// the content to scale and shift
	boost::shared_ptr<Painter> _content;
};

} // namespace gui

#endif // GUI_ZOOM_PAINTER_H__

