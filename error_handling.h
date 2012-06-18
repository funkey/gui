/*
 *       Filename:  error_handling.h
 *
 *    Description:  Provides a macro for opengl error handling.
 *
 *        Version:  1.0
 *        Created:  12.02.2008 16:34:51
 *
 *         Author:  Jan Funke, TU Dresden
 */

#ifndef __ERROR_HANDLING_H
#define __ERROR_HANDLING_H

#include <assert.h>
#include "util/Logger.h"

  #ifndef NDEBUG

    #define GL_ASSERT assert(glCheckHealth())

  #else

    #define GL_ASSERT

  #endif // NDEBUG

  bool glCheckHealth();

#endif // __ERROR_HANDLING_H

