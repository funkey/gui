#ifndef GUI_NUMBER_VIEW_H__
#define GUI_NUMBER_VIEW_H__

#include <pipeline/all.h>
#include <gui/TextPainter.h>
#include <gui/Signals.h>

namespace gui {

template <typename Precision>
class NumberView : public pipeline::SimpleProcessNode<> {

public:

	/**
	 * Create a new number view.
	 *
	 * @param precision The number of digits to show after the comma.
	 */
	NumberView(int precision = 2);

private:

	void updateOutputs();

	pipeline::Input<Precision>    _value;
	pipeline::Output<TextPainter> _painter;

	signals::Slot<const SizeChanged> _sizeChanged;

	// the number of digits to show after the comma
	int _precision;
};

template <typename Precision>
NumberView<Precision>::NumberView(int precision) :
	_precision(precision) {

	registerInput(_value, "value");
	registerOutput(_painter, "painter");

	_painter.registerForwardSlot(_sizeChanged);
}

template <typename Precision>
void
NumberView<Precision>::updateOutputs() {

	std::ostringstream ss;
	ss << std::fixed << std::setprecision(_precision) << *_value;

	_painter->setText(ss.str());

	_sizeChanged();
}

} // namespace gui

#endif // GUI_NUMBER_VIEW_H__

