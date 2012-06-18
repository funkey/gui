#ifndef GL_CONTEXT_H__
#define GL_CONTEXT_H__

// TODO: let cmake figure that out
#define SYSTEM_LINUX 1

// include platform dependent implementations
#if defined(SYSTEM_LINUX) || defined(SYSTEM_FREEBSD)

	#include <gui/linux/GlxContext.h>

#elif defined(SYSTEM_WINDOWS)

	#include <gui/windows/WglContext.h>

#elif defined(SYSTEM_OSX)

	#include <gui/osx/CocoaContext.h>

#endif

#endif // GL_CONTEXT_H__

