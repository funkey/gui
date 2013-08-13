#include "Texture.h"

namespace gui {

logger::LogChannel Texture::texturelog("texturelog", "[Texture] ");

Texture::Texture(GLsizei width, GLsizei height, GLint format) :
	_format(format),
	_width(width),
	_height(height),
	_texWidth(1),
	_texHeight(1),
	_tex(0),
	_buf(0),
	_bufferDirty(false),
	_mapped(0) {

	// make sure we have a valid OpenGl context
	OpenGl::Guard guard;

	// create the OpenGl texture
	glCheck(glGenTextures(1, &_tex));

	// create a pixel buffer object for the texture
	glCheck(glGenBuffers(1, &_buf));

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
	glCheck(glDeleteBuffers(1, &_buf));

	// delete texture
	glCheck(glDeleteTextures(1, &_tex));
}

void
Texture::resize(GLsizei width, GLsizei height) {

	// make sure we have a valid OpenGl context
	OpenGl::Guard guard;

	_width  = width;
	_height = height;

	// bind buffer
	glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _buf));

	// create new buffer
	unsigned int size = _width*_height;
	if (_format == GL_RGB)
		size *= 3;
	else if (_format == GL_RGBA)
		size *= 4;
	glCheck(glBufferData(GL_PIXEL_UNPACK_BUFFER, size, 0, GL_DYNAMIC_DRAW));

	// unbind buffer
	glCheck(glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0));

	// bind texture
	glCheck(glBindTexture(GL_TEXTURE_2D, _tex));

	// set size of texture
	glCheck(glTexImage2D(GL_TEXTURE_2D, 0, _format, _width, _height, 0, GL_RGB, GL_FLOAT, 0));

	// unbind texture
	glCheck(glBindTexture(GL_TEXTURE_2D, 0));
}

void
Texture::bind() {

	// bind texture
	glCheck(glBindTexture(GL_TEXTURE_2D, _tex));
}

void
Texture::unbind() {

	// unbind texture
	glCheck(glBindTexture(GL_TEXTURE_2D, 0));
}

void
Texture::loadData(const Buffer& buffer, int xoffset, int yoffset, float scale, float bias) {

	if (buffer.width() > _width - xoffset || buffer.height() > _height - yoffset) {

		LOG_ERROR(texturelog)
				<< "size of buffer doesn't match size of texture: texture is of size "
				<< _width << "x" << _height << ", buffer is "
				<< buffer.width() << "x" << buffer.height()
				<< " and offset is (" << xoffset << ", " << yoffset << ")" << std::endl;
		return;
	}

	// make sure we have a valid OpenGl context
	OpenGl::Guard guard;

	////////////////////
	// update texture //
	////////////////////

	// bind texture
	bind();

	// bind buffer
	buffer.bind();

	// set color/intensity scale and bias
	glCheck(glPixelTransferf(GL_RED_SCALE,   scale));
	glCheck(glPixelTransferf(GL_GREEN_SCALE, scale));
	glCheck(glPixelTransferf(GL_BLUE_SCALE,  scale));
	glCheck(glPixelTransferf(GL_RED_BIAS,    bias));
	glCheck(glPixelTransferf(GL_GREEN_BIAS,  bias));
	glCheck(glPixelTransferf(GL_BLUE_BIAS,   bias));

	// update texture
	LOG_ALL(texturelog)
			<< "updating subimage "
			<< _width << "x" << _height << ", buffer is "
			<< buffer.width() << "x" << buffer.height()
			<< " and offset is (" << xoffset << ", " << yoffset << ")" << std::endl;
	glCheck(glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, buffer.width(), buffer.height(), buffer.getFormat(), buffer.getType(), 0));

	// set color/intensity scale and bias
	glCheck(glPixelTransferf(GL_RED_SCALE,   1.0));
	glCheck(glPixelTransferf(GL_GREEN_SCALE, 1.0));
	glCheck(glPixelTransferf(GL_BLUE_SCALE,  1.0));
	glCheck(glPixelTransferf(GL_RED_BIAS,    0.0));
	glCheck(glPixelTransferf(GL_GREEN_BIAS,  0.0));
	glCheck(glPixelTransferf(GL_BLUE_BIAS,   0.0));

	// unbind texture
	unbind();

	// unbind buffer
	buffer.unbind();

	// the texture was changed, but _buf wasn't
	_bufferDirty = true;
}

} // namespace gui
