#ifndef __TEXTURE_H
#define __TEXTURE_H

#include <algorithm>
#include <iostream>
#include <iterator>

#include <gui/OpenGl.h>
#include <gui/OpenGlTraits.h>

#include "Buffer.h"

namespace gui {

class Texture {

public:

	/**
	 * Default constructor.
	 *
	 * Creates a texture of the given size and format.
	 *
	 * @param width The width of the texture.
	 * @param height The height of the texture.
	 * @param format The internal format of the texture (GL_RGB[A],
	 *               GL_LUMINANCE, ...)
	 */
	Texture(GLsizei data_width, GLsizei data_height, GLint format);

	/**
	 * Frees the texture and pixel buffer object.
	 */
	virtual ~Texture();
	
	/**
	 * Load texture data from an array of values.
	 *
	 * @param begin An input iterator to the beginning of the data. The caller
	 *              has to make sure that the data contains at least
	 *              width*height elements. Each element is supposed to represent
	 *              a pixel, e.g., for RGB each element shoud contain
	 *              three values.
	 */
	template <typename InputIterator>
	void loadData(InputIterator begin, float scale = 1.0f, float bias = 0.0f);

	/**
	 * Load texture data from a buffer.
	 */
	void loadData(const Buffer& buffer, int offsetx, int offsety, float scale = 1.0f, float bias = 0.0f);

	/**
	 * Bind this texture. Calls glBindTexture().
	 */
	void bind();

	/**
	 * Unbind this texture.
	 */
	void unbind();

	/**
	 * Resize the texture, if needed.
	 *
	 * @param width The new width.
	 * @param height The new height.
	 */
	void resize(GLsizei width, GLsizei height);

	/**
	 * @return The width of the texture in pixels.
	 */
	inline GLsizei width() const { return _width; };

	/**
	 * @return The height of the texture in pixels.
	 */
	inline GLsizei height() const { return _height; };

	/**
	 * Map the texture's content to accessible device memory for direct 
	 * rendering. Don't forget to call unmap() when done.
	 */
	template <typename PixelType>
	PixelType* map();

	/**
	 * Unmap this texture.
	 */
	template <typename PixelType>
	void unmap(float scale = 1.0f, float bias = 0.0f);

private:

	// the OpenGL target of the texture
	GLenum _target;
	
	// the internal format
	GLint _format;

	// the size of the visible area in pixels
	GLsizei _width;
	GLsizei _height;

	// the texture coordinate limits (to be used with glTexCoord2d)
	GLdouble _texWidth;
	GLdouble _texHeight;

	// the internal OpenGL id of the texture
	GLuint _tex;

	// the internal OpenGL id of the buffer object
	GLuint _buf;

	// pointer to the mapped memory of this texture
	void* _mapped;
};

/*****************
 * IMPLEMENTAION *
 *****************/

template <typename PixelType>
PixelType*
Texture::map() {

	// bind buffer
	glCheck(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _buf));

	// map the pixel buffer object
	_mapped = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);

	return (PixelType*)_mapped;
}

template <typename PixelType>
void
Texture::unmap(float scale, float bias) {

	// get the appropriate OpenGL format and type for this pixel type
	GLenum format = detail::pixel_format_traits<PixelType>::gl_format;
	GLenum type   = detail::pixel_format_traits<PixelType>::gl_type;

	if (_mapped) {

		_mapped = 0;

		// unmap the pixel buffer object
		glCheck(glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB));

		// bind texture
		glCheck(glBindTexture(GL_TEXTURE_2D, _tex));

		// bind buffer
		glCheck(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _buf));

		// set color/intensity scale and bias
		glCheck(glPixelTransferf(GL_RED_SCALE,   scale));
		glCheck(glPixelTransferf(GL_GREEN_SCALE, scale));
		glCheck(glPixelTransferf(GL_BLUE_SCALE,  scale));
		glCheck(glPixelTransferf(GL_RED_BIAS,    bias));
		glCheck(glPixelTransferf(GL_GREEN_BIAS,  bias));
		glCheck(glPixelTransferf(GL_BLUE_BIAS,   bias));

		// update texture
		glCheck(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, format, type, 0));

		// set color/intensity scale and bias
		glCheck(glPixelTransferf(GL_RED_SCALE,   1.0));
		glCheck(glPixelTransferf(GL_GREEN_SCALE, 1.0));
		glCheck(glPixelTransferf(GL_BLUE_SCALE,  1.0));
		glCheck(glPixelTransferf(GL_RED_BIAS,    0.0));
		glCheck(glPixelTransferf(GL_GREEN_BIAS,  0.0));
		glCheck(glPixelTransferf(GL_BLUE_BIAS,   0.0));

		// unbind texture
		glCheck(glBindTexture(GL_TEXTURE_2D, 0));
	}

	// unbind buffer
	glCheck(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));
}

template <typename InputIterator>
void
Texture::loadData(InputIterator data, float scale, float bias) {

	// get iterator value type (which is the pixel type)
	typedef typename std::iterator_traits<InputIterator>::value_type pixel_type;

	// make sure we have a valid OpenGl context
	OpenGl::Guard guard;

	pixel_type* p = map<pixel_type>();

	if (p) {

		// copy the data
		std::copy(data, data + _width*_height, p);
	}

	unmap<pixel_type>(scale, bias);
}

} // namespcae gui

#endif // #ifndef __TEXTURE_H
