#ifndef GUI_KEY_SIGNALS_H__
#define GUI_KEY_SIGNALS_H__

#include "GuiSignals.h"
#include <gui/Keys.h>
#include <gui/Modifiers.h>

namespace gui {

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

} // namespace gui

#endif // GUI_KEY_SIGNALS_H__

