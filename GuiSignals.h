#ifndef GUI_SIGNALS_H__
#define GUI_SIGNALS_H__

#include <signals/Signal.h>
#include <util/rect.hpp>

namespace gui {

/**
 * Base class for all gui signals.
 */
class GuiSignal : public signals::Signal {};

/**
 * Indicates a change of the content of a gui element.
 */
class ContentChanged : public GuiSignal { public: ContentChanged() {} };

/**
 * SizeChanged is a ContentChanged, since the size of a gui element cannot
 * change without a change in the content.
 */
class SizeChanged : public ContentChanged {

public:

	SizeChanged() {}

	SizeChanged(const util::rect<double>& newSize) :
		_newSize(newSize) {}

	const util::rect<double>& getNewSize() {

		return _newSize;
	}

private:

	util::rect<double> _newSize;
};

/**
 * A resize signal, to be sent downstream to request the resize of painters.
 */
class Resize : public GuiSignal {

public:

	Resize() {}

	Resize(const util::rect<double>& toSize) :
		_toSize(toSize) {}

	const util::rect<double>& getSize() const {

		return _toSize;
	}

private:

	util::rect<double> _toSize;
};

/**
 * Base class for user input signals.
 */
class InputSignal : public GuiSignal {

public:

	InputSignal() :
		processed(false),
		timestamp(0) {}

	InputSignal(unsigned long timestamp_) :
		processed(false),
		timestamp(timestamp_) {}

	bool processed;

	unsigned long timestamp;
};

} // namespace gui

#endif // GUI_SIGNALS_H__

