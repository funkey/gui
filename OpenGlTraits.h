#ifndef OPENGL_TRAITS_H__
#define OPENGL_TRAITS_H__

#include <boost/array.hpp>

#include <config.h>

#ifdef HAVE_VIGRA
#include <vigra/stdimage.hxx>
#endif

#ifdef HAVE_CAIRO
#include <gui/Cairo.h>
#endif

namespace gui {

namespace detail {

/**
 * Data type traits. Finds the corresponding OpenGl data type to a pixel value
 * type.
 */

// default
template <typename DataType>
struct pixel_type_traits {

};

template <>
struct pixel_type_traits<unsigned char> {

	enum { gl_type = GL_UNSIGNED_BYTE };
};

template <>
struct pixel_type_traits<double> {

	// OpenGl does not support that -- consider using converter<float> on your
	// image
};

template <>
struct pixel_type_traits<float> {

	enum { gl_type = GL_FLOAT };
};

/**
 * Pixel type traits. Finds the corresponding OpenGl pixel type to a various
 * pixel types.
 */

// default: GL_LUMINANCE
template <typename PixelType>
struct pixel_format_traits {

	typedef PixelType value_type;

	enum { gl_format = GL_LUMINANCE };
	enum { gl_type   = pixel_type_traits<value_type>::gl_type };
};

#ifdef HAVE_VIGRA

// specialisation: GL_RGB
template <typename ValueType>
struct pixel_format_traits<vigra::RGBValue<ValueType> > {

	typedef ValueType value_type;

	enum { gl_format = GL_RGB };
	enum { gl_type   = pixel_type_traits<value_type>::gl_type };
};

#endif

#ifdef HAVE_CAIRO

// specialisation: GL_BGRA
template <>
struct pixel_format_traits<cairo_pixel_t> {

	typedef unsigned char      value_type;

	enum { gl_format = GL_BGRA };
	enum { gl_type   = GL_UNSIGNED_BYTE };
};

#endif

// specialisation: boost::array<???, 4>
template <typename ValueType>
struct pixel_format_traits<boost::array<ValueType, 4> > {

	typedef ValueType value_type;

	enum { gl_format = GL_RGBA };
	enum { gl_type   = pixel_type_traits<value_type>::gl_type };
};

} // namspace detail

} // namespace gui

#endif // OPENGL_TRAITS_H__

