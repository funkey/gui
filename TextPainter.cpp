#include <iostream>

#include <util/Logger.h>
#include <exceptions.h>
#include "TextPainter.h"

using std::string;
using std::endl;
using namespace logger;

namespace gui {

LogChannel textpainterlog("textpainterlog", "[TextPainter] ");

TextPainter::TextPainter(string text) :
	_text(text),
	_textSize(20.0),
	_cairoTextSize(0),
	_cairoWidth(1),
	_cairoHeight(1),
	_padding(_textSize/2),
	_textColor(4, 1.0),
	_context(0),
	_surface(0),
	_lastResolution(1, 1),
	_lastRoi(0, 0, 0, 0) {

	// ensure a valid opengl context
	OpenGl::Guard guard;

	// create a pixel buffer object
	glCheck(glGenBuffersARB(1, &_buf));

	// create cairo font options
	_fontOptions = cairo_font_options_create();

	// override system default antialiasing method
	cairo_font_options_set_antialias(_fontOptions, CAIRO_ANTIALIAS_DEFAULT);
}

TextPainter::~TextPainter() {

	if (_context)
		cairo_destroy(_context);

	if (_surface)
		cairo_surface_destroy(_surface);

	if (_fontOptions)
		cairo_font_options_destroy(_fontOptions);

	// ensure a valid opengl context
	OpenGl::Guard guard;

	// delete pixel buffer object
	glCheck(glDeleteBuffersARB(1, &_buf));
}

void
TextPainter::draw(
		const util::rect<double>&  roi,
		const util::point<double>& resolution) {

	// only update texture if necessary
	//if (resolution != _lastResolution || roi != _lastRoi [> TODO: make this more efficient <]) {

		LOG_ALL(textpainterlog) << "need update" << endl;

		_lastRoi        = roi;
		_lastResolution = resolution;

		redrawText(roi, resolution);

		// if there is nothing to do, don't do it
		if (_glRoi.width() <= 0 || _glRoi.height() <= 0)
			return;

		LOG_ALL(textpainterlog) << "(re)loaded texture" << endl;
	//}

	drawText();
}

void
TextPainter::computeSize(
		const util::rect<double>&  roi,
		const util::point<double>& resolution) {

	LOG_ALL(textpainterlog) << "[computeSize] computing size..." << std::endl;

	// TODO: we call this just to get a valid context -- create a dummy context
	// instead and use that
	_glRoi.minX  = 0;
	_glRoi.minY  = 0;
	_cairoWidth  = 1;
	_cairoHeight = 1;

	if (!prepareBuffer()) {

		LOG_DEBUG(textpainterlog) << "[computeSize] failed to create dummy buffer" << std::endl;
		return;
	}

	// set font and color
	LOG_ALL(textpainterlog) << "[computeSize] drawing cairo text with size " << _textSize << std::endl;
	_cairoTextSize = _textSize;
	setFont(_context, _textColor);

	// ncu ... normalized cairo units
	//  gu ... open gl units
	//  cu ... scaled cairo units

	// Determine the extents of the text to draw in cairo pixels, when cairo
	// text size is set to textSize. [ncu]
	cairo_text_extents(_context, _text.c_str(), &_extents);

	finishBuffer();

	util::rect<double> textSize(0.0, 0.0, _extents.width, _extents.height);

	LOG_ALL(textpainterlog) << "[computeSize] text would have size " << textSize << ", when drawn with " << _cairoTextSize << std::endl;

	// Add some padding. [ncu]
	textSize.minX -= _padding;
	textSize.minY -= _padding;
	textSize.maxX += _padding;
	textSize.maxY += _padding;

	LOG_ALL(textpainterlog) << "[computeSize] with some padding of " << _padding << " [ncu], this is " << textSize << "[ncu]" << std::endl;

	// Now we have to determine the target size according to the current
	// resolution. [cu]

	// Let's say one cairo pixel is one Gl unit. We need to know the target
	// size in Gl units. [ncu] -> [gu]

	_glSize    = textSize;
	_glPadding = util::point<double>(_padding, _padding);

	LOG_ALL(textpainterlog) << "[computeSize] in Gl coordinates " << _glSize << "[gu]" << std::endl;

	// Despite the possible ROI cropping, our size is glSize
	setSize(_glSize.minX, _glSize.minY, _glSize.maxX, _glSize.maxY);
}

void
TextPainter::redrawText(const util::rect<double>& roi, const util::point<double>& resolution) {

	// make a copy of the data needed to draw the text
	string __text;
	double __textSize;
	double __padding;
	std::vector<double> __textColor;

	{
		boost::mutex::scoped_lock lock(_dataMutex);

		__text      = _text;
		__textSize  = _textSize;
		__padding   = _padding;
		__textColor = _textColor;
	}

	LOG_ALL(textpainterlog) << "[redrawText] redrawing text..." << std::endl;

	// Each Gl unit represents resolution.* pixels. This gives the target pixel
	// size. [cu]

	util::rect<double>  cairoSize        = _glSize*resolution;
	util::point<double> cairoPadding     = _glPadding*resolution;
	util::point<double> glToCairoScale   = resolution;
	util::point<double> textToGlScale(1.0, 1.0);
	util::point<double> textToCairoScale = textToGlScale*glToCairoScale;

	LOG_ALL(textpainterlog) << "[redrawText] according to current resolution ("
	                     << resolution << "), this is " << cairoSize << "[cu]"
	                     << std::endl;

	_glRoi = roi;

	LOG_ALL(textpainterlog) << "[redrawText] ROI is " << _glRoi << "[gu]" << std::endl;

	// Find portion of target region that is within ROI. [gu]
	_glRoi.minX = std::max(_glRoi.minX, _glSize.minX);
	_glRoi.minY = std::max(_glRoi.minY, _glSize.minY);
	_glRoi.maxX = std::min(_glRoi.maxX, _glSize.maxX);
	_glRoi.maxY = std::min(_glRoi.maxY, _glSize.maxY);

	// Determine ROI region of target region. [cu]
	util::rect<double> cairoRoi = _glRoi*glToCairoScale;

	LOG_ALL(textpainterlog) << "[redrawText] relevant part is " << cairoRoi << "[cu]" << std::endl;

	if (cairoRoi.width() <= 0 || cairoRoi.height() <= 0)
		return;

	// The target size. [cu]
	_cairoWidth  = (int)round(cairoRoi.width());
	_cairoHeight = (int)round(cairoRoi.height());

	LOG_ALL(textpainterlog) << "[redrawText] rounded, this is " << _cairoWidth << "x"
	                     << _cairoHeight << " pixels" << std::endl;

	LOG_ALL(textpainterlog) << "[redrawText] scaling is " << textToCairoScale << std::endl;

	// Next, we prepare the cairo surface for the text.
	if (!prepareBuffer()) {

		LOG_DEBUG(textpainterlog) << "[redrawText] failed to create buffer" << std::endl;
		return;
	}

	LOG_ALL(textpainterlog) << "[redrawText] prepared new cairo surface" << std::endl;

	// Scale the cairo text to fill the scaled surface.
	_cairoTextSize = __textSize*textToCairoScale.x; // this will be incorrect for anisotropic resolutions
	setFont(_context, __textColor);

	LOG_ALL(textpainterlog) << "[redrawText] cairo text size is now " << _cairoTextSize << std::endl;

	// Determine the starting point of the text drawing with respect to padding.
	// [ncu]
	util::point<double> textStart(-_extents.x_bearing, -_extents.y_bearing);

	LOG_ALL(textpainterlog) << "[redrawText] text starting point according to bearing is " << textStart << "[ncu]" << std::endl;

	textStart += util::point<double>(_padding, _padding);

	LOG_ALL(textpainterlog) << "[redrawText] with padding this makes " << textStart << "[ncu]" << std::endl;

	// Determine the starting point of the text drawing relative to ROI [cu].
	util::point<double> cairoRoiStart = textStart*textToCairoScale;

	LOG_ALL(textpainterlog) << "[redrawText] in target units this is " << cairoRoiStart << "[cu]" << std::endl;

	cairoRoiStart -= util::point<double>(cairoRoi.minX - cairoSize.minX, cairoRoi.minY - cairoSize.minY);

	LOG_ALL(textpainterlog) << "[redrawText] taking into account the ROI, we have finally " << cairoRoiStart << "[cu]" << std::endl;

	// We have to draw upside down to be OpenGl compatible
	cairo_scale(_context, 1.0, -1.0);

	// Move to the starting point.
	cairo_move_to(
			_context,
			 cairoRoiStart.x,
			 cairoRoiStart.y - _cairoHeight);

	// Draw the text.
	cairo_show_text(_context, __text.c_str());

	finishBuffer();
}

void
TextPainter::setFont(cairo_t* context, const std::vector<double>& color) {

	cairo_select_font_face(
			context,
			"sans-serif",
			CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_NORMAL);

	cairo_set_font_size(
			context,
			_cairoTextSize);

	cairo_set_source_rgba(
			context,
			color[0],
			color[1],
			color[2],
			color[3]);
}

bool
TextPainter::prepareBuffer() {

	if (_cairoWidth <= 0 || _cairoHeight <= 0)
		return false;

	if (_context)
		cairo_destroy(_context);

	if (_surface)
		cairo_surface_destroy(_surface);

	// ensure a valid opengl context
	OpenGl::Guard guard;

	// bind buffer for writing
	glCheck(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _buf));

	// discard previous buffer (so we don't have to wait for GPU until we can
	// map) and create new buffer
	glCheck(glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, _cairoWidth*_cairoHeight*sizeof(cairo_pixel_t), 0, GL_DYNAMIC_DRAW));

	// bind buffer for reading
	glCheck(glBindBufferARB(GL_PIXEL_PACK_BUFFER_ARB, _buf));

	LOG_ALL(textpainterlog) << "determine raster position of "
	                        << _glRoi.minX + 1.0/_lastResolution.x << ", "
	                        << _glRoi.maxY - 1.0/_lastResolution.y << " [gl]" << std::endl;

	// get the window coordinates of the lower-left corner of the rectangle we
	// want to draw to
	int r[4];
	glCheck(glRasterPos2f(_glRoi.minX + 1.0/_lastResolution.x, _glRoi.maxY - 1.0/_lastResolution.y));
	glCheck(glGetIntegerv(GL_CURRENT_RASTER_POSITION, r));
	_rasterPos.x = r[0];
	_rasterPos.y = r[1];

	LOG_ALL(textpainterlog) << "read content from raster position " << _rasterPos << std::endl;

	// load current content to buffer
	glCheck(glReadPixels(
			_rasterPos.x,
			_rasterPos.y,
			_cairoWidth,
			_cairoHeight,
			detail::pixel_format_traits<cairo_pixel_t>::gl_format,
			detail::pixel_format_traits<cairo_pixel_t>::gl_type,
			0));

	// map the pixel buffer object
	unsigned char* p = (unsigned char*)glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);

	// wrap the buffer in a cairo surface
	_surface =
			cairo_image_surface_create_for_data(
					p,
					CAIRO_FORMAT_ARGB32,
					_cairoWidth,
					_cairoHeight,
					cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, _cairoWidth));

	// create a context for the surface
	_context = cairo_create(_surface);

	// set antialiasing options
	cairo_set_font_options(_context, _fontOptions);

	return true;
}

void
TextPainter::finishBuffer() {

	// ensure a valid opengl context
	OpenGl::Guard guard;

	// unmap the pixel buffer object
	glCheck(glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB));

	// unbind buffer
	glCheck(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));
}

void
TextPainter::drawText() {

	// ensure a valid opengl context
	OpenGl::Guard guard;

	// bind buffer
	glCheck(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _buf));

	// enable alpha blending
	glEnable(GL_BLEND);
	glCheck(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	// unbind any buffer or texture that might fly around...
	glCheck(glBindTexture(GL_TEXTURE_2D, 0));

	// set raster position
	glCheck(glWindowPos2i(_rasterPos.x, _rasterPos.y));

	LOG_ALL(textpainterlog) << "draw text at raster position " << _rasterPos << std::endl;

	// draw the pixels directly on the frame buffer
	glCheck(glDrawPixels(
			_cairoWidth,
			_cairoHeight,
			detail::pixel_format_traits<cairo_pixel_t>::gl_format,
			detail::pixel_format_traits<cairo_pixel_t>::gl_type,
			0));

	//glColor4f(1.0, 0.5, 0.5, 0.75);
	//glBegin(GL_QUADS);
	//glVertex2f(_glRoi.minX, _glRoi.minY);
	//glVertex2f(_glRoi.maxX, _glRoi.minY);
	//glVertex2f(_glRoi.maxX, _glRoi.maxY);
	//glVertex2f(_glRoi.minX, _glRoi.maxY);
	//glEnd();

	// clean up
	glDisable(GL_BLEND);

	// unbind buffer
	glCheck(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));
}

void
TextPainter::setText(std::string text) {

	{
		boost::mutex::scoped_lock lock(_dataMutex);

		_text = text;

		computeSize(_lastRoi, _lastResolution);
	}
}

void
TextPainter::setTextSize(double size) {

	{
		boost::mutex::scoped_lock lock(_dataMutex);

		_textSize = size;
		_padding  = size/2;

		computeSize(_lastRoi, _lastResolution);
	}
}

void
TextPainter::setTextColor(double r, double g, double b, double a) {

	{
		boost::mutex::scoped_lock lock(_dataMutex);

		_textColor[0] = r;
		_textColor[1] = g;
		_textColor[2] = b;
		_textColor[3] = a;
	}
}

} // namespace gui
