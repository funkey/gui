#include "Texture.h"

namespace gui {

Texture::Texture(GLsizei width, GLsizei height, GLint format) :
	_format(format),
	_width(width),
	_height(height),
	_texWidth(1),
	_texHeight(1),
	_tex(0),
	_buf(0) {

	// make sure we have a valid OpenGl context
	OpenGl::Guard guard;

	// create the OpenGl texture
	glCheck(glGenTextures(1, &_tex));

	// create a pixel buffer object for the texture
	glCheck(glGenBuffersARB(1, &_buf));

	// setup texture
	glCheck(glBindTexture(GL_TEXTURE_2D, _tex));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));

	// resize texture and buffer
	resize(_width, _height);
}

Texture::~Texture()
{
	// make sure we have a valid OpenGl context
	OpenGl::Guard guard;

	// delete buffer
	glCheck(glDeleteBuffersARB(1, &_buf));

	// delete texture
	glCheck(glDeleteTextures(1, &_tex));
}

void
Texture::resize(GLsizei width, GLsizei height) {

	// make sure we have a valid OpenGl context
	OpenGl::Guard guard;

	_width  = width;
	_height = height;

	// unbind buffer
	glCheck(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));

	// bind texture
	glCheck(glBindTexture(GL_TEXTURE_2D, _tex));

	// set size of texture
	glCheck(glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, GL_RGB, GL_FLOAT, 0));

	// unbind texture
	glCheck(glBindTexture(GL_TEXTURE_2D, 0));
}

void
Texture::bind() {

	// make sure we have a valid OpenGl context
	OpenGl::Guard guard;

	// bind texture
	glCheck(glBindTexture(GL_TEXTURE_2D, _tex));
}

} // namespace gui
