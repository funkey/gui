#ifndef GUI_GL_CONTEXT_CREATOR_H__
#define GUI_GL_CONTEXT_CREATOR_H__

namespace gui {

// forward declaration
class GlContext;

/**
 * Interface for classes that are able to create GlContexts.
 */
class GlContextCreator {

public:

	virtual GlContext* createGlContext() = 0;
};

} // namespace gui

#endif // GUI_GL_CONTEXT_CREATOR_H__

