#ifndef GUI_PEN_SIGNALS_H__
#define GUI_PEN_SIGNALS_H__

#include "PointerSignal.h"
#include <gui/Buttons.h>
#include <gui/Modifiers.h>

namespace gui {

class PenSignal : public PointerSignal {

public:

	PenSignal() :
		pressure(0.75),
		modifiers(NoModifier) {}

	PenSignal(const util::point<double>& position_, double pressure_, const Modifiers& modifiers_) :
		PointerSignal(position_),
		pressure(pressure_),
		modifiers(modifiers_) {}

	double pressure;

	Modifiers modifiers;
};

class PenMove : public PenSignal {

public:

	PenMove() {}

	PenMove(
			const util::point<double>& position,
			double pressure,
			const Modifiers& modifiers) :

		PenSignal(position, pressure, modifiers) {}
};

class PenDown : public PenSignal {

public:

	PenDown() :
		button(buttons::NoButton) {}

	PenDown(
			const buttons::Button& button_,
			const util::point<double>& position,
			double pressure,
			const Modifiers& modifiers) :

		PenSignal(position, pressure, modifiers),
		button(button_) {}

	buttons::Button button;
};

class PenUp   : public PenSignal {

public:

	PenUp() {}

	PenUp(
			const buttons::Button& button_,
			const util::point<double>& position,
			double pressure,
			const Modifiers& modifiers) :

		PenSignal(position, pressure, modifiers),
		button(button_) {}

	buttons::Button button;
};

} // namespace gui

#endif // GUI_PEN_SIGNALS_H__

