#ifndef CAIRO_H__
#define CAIRO_H__

/**
 * Wrappers to use the cairo surface type as an image.
 */

namespace gui {

struct cairo_pixel_t {

	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char alpha;
};

struct cairo_image_adaptor {

	typedef cairo_pixel_t value_type;

	cairo_image_adaptor(
			unsigned char* data_,
			unsigned int width_,
			unsigned int height_) :
		data(data_),
		width(width_),
		height(height_) {}

	const value_type operator()(unsigned int x, unsigned int y) const {

		value_type pixel;

		pixel.blue  = data[(y*width + x)*4];
		pixel.green = data[(y*width + x)*4 + 1];
		pixel.red   = data[(y*width + x)*4 + 2];
		pixel.alpha = data[(y*width + x)*4 + 3];

		return pixel;
	}

	unsigned char* data;

	unsigned int   width;

	unsigned int   height;
};

} // namespace gui

#endif // CAIRO_H__

