#include "Texture.h"

namespace gui {

logger::LogChannel Texture::texturelog("texturelog", "[Texture] ");

Texture::Texture(GLsizei width, GLsizei height, GLint format) :
	_format(format),
	_width(width),
	_height(height),
	_texWidth(1),
	_texHeight(1),
	_tex(0) {

	// make sure we have a valid OpenGl context
	OpenGl::Guard guard;

	// create the OpenGl texture
	glCheck(glGenTextures(1, &_tex));

	// setup texture
	glCheck(glBindTexture(GL_TEXTURE_2D, _tex));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP));
	glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP));

	// resize texture
	resize(_width, _height);
}

Texture::~Texture()
{
	// make sure we have a valid OpenGl context
	OpenGl::Guard guard;

	// delete texture
	glCheck(glDeleteTextures(1, &_tex));
}

void
Texture::resize(GLsizei width, GLsizei height) {

	// make sure we have a valid OpenGl context
	OpenGl::Guard guard;

	_width  = width;
	_height = height;

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
}

} // namespace gui
