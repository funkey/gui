/*
 *       Filename:  error_handling.cpp
 *
 *    Description:  Implementation for error_handling.h
 *
 *        Version:  1.0
 *        Created:  12.02.2008 16:53:46
 *
 *         Author:  Jan Funke
 */

#include "OpenGl.h"
#include "error_handling.h"

bool glCheckHealth() {

  GLenum error = glGetError();

  if (error) {
    logger::out(logger::error) << "OpenGL error: " << gluErrorString(error) << std::endl;
    return false;
  }

  return true;
}

