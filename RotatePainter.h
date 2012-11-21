#ifndef GUI_ROTATE_PAINTER_H__
#define GUI_ROTATE_PAINTER_H__

#include <gui/Painter.h>

namespace gui {

class RotatePainter : public Painter {

public:

	RotatePainter();

	void setContent(boost::shared_ptr<Painter> content);

	void setRotation(double x, double y, double z, double w);

	void draw(const util::rect<double>& roi, const util::point<double>& resolution);

	void updateSize();

	void setHighlight(bool highlight) { _highlight = highlight; }

private:

	// the rotation parameters
	double _x, _y, _z, _w;

	double _centerX, _centerY, _centerZ;

	// the content to rotate
	boost::shared_ptr<Painter> _content;

	bool _highlight;
};

} // namespace gui

#endif // GUI_ROTATE_PAINTER_H__

