#ifndef CONTEXT_SETTINGS_H__
#define CONTEXT_SETTINGS_H__

namespace gui {

struct ContextSettings {

	explicit ContextSettings(
			unsigned int depthBits_ = 0,
			unsigned int stencilBits_ = 0,
			unsigned int antialiasingLevel_ = 0,
			unsigned int majorVersion_ = 2,
			unsigned int minorVersion_ = 9,
			bool         vsync_ = true) :
		depthBits(depthBits_),
		stencilBits(stencilBits_),
		antialiasingLevel(antialiasingLevel_),
		majorVersion(majorVersion_),
		minorVersion(minorVersion_),
		vsync(vsync_) {}

	unsigned int depthBits;
	unsigned int stencilBits;
	unsigned int antialiasingLevel;
	unsigned int majorVersion;
	unsigned int minorVersion;
	bool         vsync;
};

} // namespace

#endif // CONTEXT_SETTINGS_H__

