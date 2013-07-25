#ifndef GUI_SLIDER_PAINTER_H__
#define GUI_SLIDER_PAINTER_H__

#include <util/rect.hpp>
#include <gui/Painter.h>

namespace gui {

class SliderPainter : public Painter {

public:

	SliderPainter(double min = 0, double max = 1, double value = 0);

	void setValue(double value);

	void setHighlight(bool highlight);

	bool draw(const util::rect<double>& roi, const util::point<double>& resolution);

	const util::rect<double>& getGraspSize();

private:

	// recompute the position of the slider
	void updateSliderPosition();

	// the current value
	double _value;

	// min and max
	double _min;
	double _max;

	// the size and location of the grasp
	util::rect<double> _graspSize;

	// indicates whether the slider should be highlighted
	bool _highlight;
};

}

#endif // GUI_SLIDER_PAINTER_H__

