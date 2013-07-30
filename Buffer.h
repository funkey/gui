#ifndef GUI_BUFFER_H__
#define GUI_BUFFER_H__

#include <gui/OpenGl.h>
#include <gui/OpenGlTraits.h>

namespace gui {

// forward declaration
class Texture;

/**
 * A class providing access to device memory.
 */
class Buffer {

public:

	/**
	 * Default constructor.
	 *
	 * Creates a buffer of the given size and format.
	 *
	 * @param width The width of the buffer.
	 * @param height The height of the buffer.
	 * @param format The internal format of the buffer (GL_RGB[A],
	 *               GL_LUMINANCE, ...)
	 * @param type The type of the data (GL_FLOAT, GL_UNSIGNED_BYTE, ...)
	 */
	Buffer(GLsizei data_width, GLsizei data_height, GLint format, GLenum type);

	/**
	 * Frees the buffer and pixel buffer object.
	 */
	virtual ~Buffer();

	/**
	 * Load this buffers data from a texture.
	 */
	void loadData(Texture& texture);
	
	/**
	 * Bind this buffer. Calls glBindBuffer().
	 */
	void bind(GLenum target = GL_PIXEL_UNPACK_BUFFER) const;

	/**
	 * Unbinds this buffer.
	 */
	void unbind(GLenum target = GL_PIXEL_UNPACK_BUFFER) const;

	/**
	 * Resize the buffer.
	 */
	void resize(GLsizei width, GLsizei height);

	/**
	 * @return The width of the buffer in pixels.
	 */
	inline GLsizei width() const { return _width; };

	/**
	 * @return The height of the buffer in pixels.
	 */
	inline GLsizei height() const { return _height; };

	/**
	 * @return The size of the buffer in bytes.
	 */
	inline GLsizei size() const { return _size; };

	inline GLint getFormat() const { return _format; };

	inline GLenum getType() const { return _type; };

	/**
	 * Map the buffer's content to accessible device memory for direct 
	 * rendering. Don't forget to call unmap() when done.
	 */
	template <typename PixelType>
	PixelType* map();

	/**
	 * Unmap this buffer.
	 */
	void unmap();

private:
	
	// the internal format
	GLint _format;

	// the GL type of this buffer's data
	GLenum _type;

	// the size of the buffer in pixels
	GLsizei _width;
	GLsizei _height;

	// the size of the buffer in bytes
	GLsizei _size;

	// the internal OpenGL id of the buffer object
	GLuint _buf;

	// pointer to the mapped memory of this buffer
	void* _mapped;
};

/*****************
 * IMPLEMENTAION *
 *****************/

template <typename PixelType>
PixelType*
Buffer::map() {

	// bind buffer
	glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _buf));

	// map the pixel buffer object
	_mapped = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

	return (PixelType*)_mapped;
}

} // namespace gui

#endif // GUI_BUFFER_H__

