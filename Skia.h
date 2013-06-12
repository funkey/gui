#ifndef GUI_SKIA_H__
#define GUI_SKIA_H__

/**
 * Wrappers to use the skia surface type as an image.
 */

namespace gui {

struct skia_pixel_t {

	unsigned char blue;
	unsigned char green;
	unsigned char red;
	unsigned char alpha;
};

struct skia_image_adaptor {

	typedef skia_pixel_t value_type;

	skia_image_adaptor(
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

#endif // GUI_SKIA_H__

