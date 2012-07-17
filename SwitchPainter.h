#ifndef GUI_SWITCH_PAINTER_H__
#define GUI_SWITCH_PAINTER_H__

#include <util/rect.hpp>
#include <gui/Painter.h>

namespace gui {

class SwitchPainter : public Painter {

public:

	SwitchPainter(bool value = false);

	void setValue(bool value);

	bool getValue();

	void setHighlight(bool highlight);

	void draw(const util::rect<double>& roi, const util::point<double>& resolution);

private:

	// the current value
	bool _value;

	// indicates whether the switch should be highlighted
	bool _highlight;
};

}

#endif // GUI_SWITCH_PAINTER_H__

