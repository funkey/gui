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

	PenSignal(
			unsigned long timestamp,
			const util::point<double>& position_,
			double pressure_,
			const Modifiers& modifiers_) :
		PointerSignal(timestamp, position_),
		pressure(pressure_),
		modifiers(modifiers_) {}

	double pressure;

	Modifiers modifiers;
};

class PenMove : public PenSignal {

public:

	PenMove() {}

	PenMove(
			unsigned long timestamp,
			const util::point<double>& position,
			double pressure,
			const Modifiers& modifiers) :
		PenSignal(timestamp, position, pressure, modifiers) {}
};

class PenDown : public PenSignal {

public:

	PenDown() :
		button(buttons::NoButton) {}

	PenDown(
			unsigned long timestamp,
			const buttons::Button& button_,
			const util::point<double>& position,
			double pressure,
			const Modifiers& modifiers) :
		PenSignal(timestamp, position, pressure, modifiers),
		button(button_) {}

	buttons::Button button;
};

class PenUp   : public PenSignal {

public:

	PenUp() {}

	PenUp(
			unsigned long timestamp,
			const buttons::Button& button_,
			const util::point<double>& position,
			double pressure,
			const Modifiers& modifiers) :
		PenSignal(timestamp, position, pressure, modifiers),
		button(button_) {}

	buttons::Button button;
};

class PenIn : public PenSignal {

public:

	PenIn() {}

	PenIn(unsigned long timestamp) :
		PenSignal(
				timestamp,
				util::point<double>(0, 0),
				0,
				gui::NoModifier) {}
};

class PenOut : public PenSignal {

public:

	PenOut() {}

	PenOut(unsigned long timestamp) :
		PenSignal(
				timestamp,
				util::point<double>(0, 0),
				0,
				gui::NoModifier) {}
};


} // namespace gui

#endif // GUI_PEN_SIGNALS_H__

