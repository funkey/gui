#ifndef GL_CONTEXT_BASE_H__
#define GL_CONTEXT_BASE_H__

#include <cstdlib>

#include <gui/ContextSettings.h>

using std::abs;

namespace gui {

class GlContextBase {

public:

	/**
	 * Make this context the active context of the calling thread. If called
	 * with <code>false</code> as argument, deactivates this context (if it was
	 * active).
	 *
	 * Platform dependent.
	 */
	virtual bool activate(bool active = true) = 0;

	/**
	 * Report whether this context is currently active.
	 *
	 * @return True, if this context is active.
	 */
	virtual bool isActive() = 0;

	/**
	 * Make visible whatever was rendered using this context.
	 *
	 * Platform dependent.
	 */
	virtual void flush() = 0;

protected:

	/**
	 * Uses a simple procedure to evalute the score of a given format to the
	 * desired one. The lower, the better.
	 */
	int evaluateFormat(
			const ContextSettings& settings,
			int depthBits,
			int stencilBits,
			int antialiasing) {

		return abs(static_cast<int>(settings.depthBits         - depthBits))   +
			   abs(static_cast<int>(settings.stencilBits       - stencilBits)) +
			   abs(static_cast<int>(settings.antialiasingLevel - antialiasing));
	}
};

} // namespace gui

#endif // GL_CONTEXT_BASE_H__

