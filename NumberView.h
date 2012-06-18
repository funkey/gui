#ifndef GUI_NUMBER_VIEW_H__
#define GUI_NUMBER_VIEW_H__

#include <pipeline/all.h>
#include <gui/TextPainter.h>
#include <gui/Signals.h>

namespace gui {

class NumberView : public pipeline::ProcessNode {

public:

	/**
	 * Create a new number view.
	 *
	 * @param precision The number of digits to show after the comma.
	 */
	NumberView(int precision = 2);

private:

	void onModified(const pipeline::Modified& signal);
	void onUpdate(const pipeline::Update& signal);

	pipeline::Input<double>       _value;
	pipeline::Output<TextPainter> _painter;

	signals::Slot<const pipeline::Modified> _modified;
	signals::Slot<const SizeChanged>        _sizeChanged;

	// the number of digits to show after the comma
	int _precision;

	bool _dirty;
};

} // namespace gui

#endif // GUI_NUMBER_VIEW_H__

