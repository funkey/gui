#ifndef GUI_FINGER_SIGNALS_H__
#define GUI_FINGER_SIGNALS_H__

#include "PointerSignal.h"
#include <gui/Buttons.h>
#include <gui/Modifiers.h>

namespace gui {

class FingerSignal : public PointerSignal {

public:

	FingerSignal() :
		id(0),
		modifiers(NoModifier) {}

	FingerSignal(const util::point<double>& position_, int id_, const Modifiers& modifiers_) :
		PointerSignal(position_),
		id(id_),
		modifiers(modifiers_) {}

	int id;

	Modifiers modifiers;
};

class FingerMove : public FingerSignal {

public:

	FingerMove() {}

	FingerMove(
			const util::point<double>& position,
			int id,
			const Modifiers& modifiers) :

		FingerSignal(position, id, modifiers) {}
};

class FingerDown : public FingerSignal {

public:

	FingerDown() :
		button(buttons::NoButton) {}

	FingerDown(
			const buttons::Button& button_,
			const util::point<double>& position,
			int id,
			const Modifiers& modifiers) :

		FingerSignal(position, id, modifiers),
		button(button_) {}

	buttons::Button button;
};

class FingerUp   : public FingerSignal {

public:

	FingerUp() {}

	FingerUp(
			const buttons::Button& button_,
			const util::point<double>& position,
			int id,
			const Modifiers& modifiers) :

		FingerSignal(position, id, modifiers),
		button(button_) {}

	buttons::Button button;
};

} // namespace gui

#endif // GUI_FINGER_SIGNALS_H__


