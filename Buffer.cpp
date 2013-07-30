#include <gui/Texture.h>
#include "Buffer.h"

namespace gui {

Buffer::Buffer(GLsizei width, GLsizei height, GLint format, GLenum type) :
	_format(format),
	_type(type),
	_width(0),
	_height(0),
	_size(0),
	_buf(0),
	_mapped(0) {

	// create a pixel buffer object for the buffer
	glCheck(glGenBuffers(1, &_buf));

	if (_buf == 0)
		BOOST_THROW_EXCEPTION(GuiError() << error_message("buffer id is zero") << STACK_TRACE);

	// resize buffer
	resize(width, height);
}

Buffer::~Buffer()
{
	OpenGl::Guard guard;

	// delete buffer
	glCheck(glDeleteBuffers(1, &_buf));
}

void
Buffer::loadData(Texture& texture) {

	// bind buffer
	glCheck(glBindBuffer(GL_PIXEL_PACK_BUFFER, _buf));

	// bind texture
	texture.bind();

	// copy texture content into buffer
	glCheck(glGetTexImage(GL_TEXTURE_2D, 0, _format, _type, 0));

	// unbind texture
	texture.unbind();

	// unbind buffer
	glCheck(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));
}

void
Buffer::resize(GLsizei width, GLsizei height) {

	_width  = width;
	_height = height;

	// bind buffer
	glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _buf));

	// create new buffer
	_size = _width*_height;
	if (_format == GL_RGB)
		_size *= 3;
	else if (_format == GL_RGBA || _format == GL_BGRA)
		_size *= 4;
	glCheck(glBufferData(GL_PIXEL_UNPACK_BUFFER, _size, 0, GL_DYNAMIC_DRAW));

	// unbind buffer
	glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));
}

void
Buffer::bind(GLenum target) const {

	// bind buffer
	glCheck(glBindBuffer(target, _buf));
}

void
Buffer::unbind(GLenum target) const {

	// unbind buffer
	glCheck(glBindBuffer(target, 0));
}

void
Buffer::unmap() {

	if (_mapped) {

		_mapped = 0;

		// unmap the pixel buffer object
		glCheck(glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER));
	}

	// unbind buffer
	glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));
}

} // namespace gui
