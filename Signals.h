#ifndef GUI_SIGNALS_H__
#define GUI_SIGNALS_H__

#include <signals/Signal.h>
#include <util/rect.hpp>
#include <gui/Keys.h>
#include <gui/Buttons.h>
#include <gui/Modifiers.h>

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

class Resize : public GuiSignal {

public:

	Resize() {}

	Resize(const util::rect<double>& toSize) :
		_toSize(toSize) {}

	const util::rect<double>& getSize() {

		return _toSize;
	}

private:

	util::rect<double> _toSize;
};

class InputSignal : public GuiSignal {

public:

	InputSignal() :
		processed(false) {}

	bool processed;
};

class KeySignal : public InputSignal {

public:

	KeySignal() :
		key(keys::NoKey) {};

	KeySignal(const keys::Key& key_, const Modifiers& modifiers_) :
		key(key_),
		modifiers(modifiers_) {}

	keys::Key key;

	Modifiers modifiers;
};

class KeyDown : public KeySignal {

public:

	KeyDown() {}

	KeyDown(const keys::Key& key, const Modifiers& modifiers) :
		KeySignal(key, modifiers) {}
};

class KeyUp : public KeySignal {

public:

	KeyUp() {}

	KeyUp(const keys::Key& key, const Modifiers& modifiers) :
		KeySignal(key, modifiers) {}
};

class MouseSignal : public InputSignal {

public:

	MouseSignal() :
		position(0, 0),
		modifiers(NoModifier) {}

	MouseSignal(const util::point<double>& position_, const Modifiers& modifiers_) :
		position(position_),
		modifiers(modifiers_) {}

	util::point<double> position;

	Modifiers modifiers;
};

class MouseMove : public MouseSignal {

public:

	MouseMove() {}

	MouseMove(
			const util::point<double>& position,
			const Modifiers& modifiers) :

		MouseSignal(position, modifiers) {}
};

class MouseDown : public MouseSignal {

public:

	MouseDown() :
		button(buttons::NoButton) {}

	MouseDown(
			const buttons::Button& button_,
			const util::point<double>& position,
			const Modifiers& modifiers) :

		MouseSignal(position, modifiers),
		button(button_) {}

	buttons::Button button;
};

class MouseUp   : public MouseSignal {

public:

	MouseUp() {}

	MouseUp(
			const buttons::Button& button_,
			const util::point<double>& position,
			const Modifiers& modifiers) :

		MouseSignal(position, modifiers),
		button(button_) {}

	buttons::Button button;
};

} // namespace gui

#endif // GUI_SIGNALS_H__

