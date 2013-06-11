#include "Buffer.h"

namespace gui {

Buffer::Buffer(GLsizei width, GLsizei height, GLint format, GLenum type) :
	_format(format),
	_type(type),
	_width(width),
	_height(height),
	_buf(0),
	_mapped(0) {

	// create a pixel buffer object for the buffer
	glCheck(glGenBuffersARB(1, &_buf));

	if (_buf == 0)
		BOOST_THROW_EXCEPTION(GuiError() << error_message("buffer id is zero") << STACK_TRACE);

	// resize buffer
	resize(_width, _height);
}

Buffer::~Buffer()
{
	// delete buffer
	glCheck(glDeleteBuffersARB(1, &_buf));
}

void
Buffer::resize(GLsizei width, GLsizei height) {

	_width  = width;
	_height = height;

	// bind buffer
	glCheck(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _buf));

	// create new buffer
	unsigned int size = _width*_height;
	if (_format == GL_RGB)
		size *= 3;
	else if (_format == GL_RGBA || _format == GL_BGRA)
		size *= 4;
	glCheck(glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, size, 0, GL_DYNAMIC_DRAW));

	// unbind buffer
	glCheck(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));
}

void
Buffer::bind() const {

	// bind buffer
	glCheck(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, _buf));
}

void
Buffer::unbind() const {

	// unbind buffer
	glCheck(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));
}

void
Buffer::unmap() {

	if (_mapped) {

		_mapped = 0;

		// unmap the pixel buffer object
		glCheck(glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB));
	}

	// unbind buffer
	glCheck(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));
}

} // namespace gui
