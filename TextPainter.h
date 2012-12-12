#ifndef TEXT_PAINTER_H__
#define TEXT_PAINTER_H__

#include <config.h>

#include <string>

#include <boost/thread.hpp>

#ifdef HAVE_CAIRO
#include <cairo.h>
#endif

#include <gui/Painter.h>
#include <gui/Texture.h>

#ifdef HAVE_CAIRO
#include <gui/Cairo.h>
#endif

namespace gui {

class TextPainter : public Painter {

public:

	TextPainter(std::string text = "");

	virtual ~TextPainter();

	virtual void draw(
			const util::rect<double>&  roi,
			const util::point<double>& resolution);

	void setText(std::string text);

	void setTextSize(double size);

	void setTextColor(double r, double g, double b, double a = 1.0);

private:

	// computes and sets the size of the text
	void computeSize(const util::rect<double>& roi, const util::point<double>& resolution);

	// redraws the text on the cairo surface
	void redrawText(const util::rect<double>& roi, const util::point<double>& resolution);

	void drawText();

	// set the cairo context font
	void setFont();

	// allocate pixel buffer object and create cairo context
	bool prepareBuffer();

	// unmap pixel buffer object
	void finishBuffer();

	// the text to draw
	std::string         _text;

	// the size of the text in OpenGl units
	double              _textSize;

	// the effective size of the text in cairo units
	double              _cairoTextSize;

	// the width and height of the cairo surface
	int                 _cairoWidth, _cairoHeight;

	// the amount by which to extend the texture (this seems neccessary since
	// cairo_extents is incorrect)
	double              _padding;

	// the color of the text (rgba)
	std::vector<double> _textColor;

	// the cairo context
	cairo_t*            _context;

	// the cairo surface to draw to
	cairo_surface_t*    _surface;

	// the cairo font options
	cairo_font_options_t* _fontOptions;

	// the extents of the text in cairo units
	cairo_text_extents_t _extents;

	// the relevant part of the text in gl units
	util::rect<double>  _glRoi;

	// the size of this painter in gl units
	util::rect<double>  _glSize;

	// the padding in gl units
	util::point<double> _glPadding;

	// the window coordinates where we start drawing
	util::point<int>    _rasterPos;

	// the last resolution for which the text was rendered
	util::point<double> _lastResolution;

	// the last roi for which the text was rendered
	util::rect<double>  _lastRoi;

	// mutex to guard changes to the cairo context
	boost::mutex        _cairoMutex;

	// OpenGl pixel buffer object
	GLuint              _buf;
};

} // namespace gui

#endif // TEXT_PAINTER_H__
