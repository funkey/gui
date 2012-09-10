#ifndef GUI_NUMBER_VIEW_H__
#define GUI_NUMBER_VIEW_H__

#include <pipeline/all.h>
#include <gui/TextPainter.h>
#include <gui/Signals.h>

namespace gui {

class NumberView : public pipeline::SimpleProcessNode {

public:

	/**
	 * Create a new number view.
	 *
	 * @param precision The number of digits to show after the comma.
	 */
	NumberView(int precision = 2);

private:

	void updateOutputs();

	pipeline::Input<double>       _value;
	pipeline::Output<TextPainter> _painter;

	signals::Slot<const SizeChanged>        _sizeChanged;

	// the number of digits to show after the comma
	int _precision;
};

} // namespace gui

#endif // GUI_NUMBER_VIEW_H__

