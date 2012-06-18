#ifndef GUI_SLIDER_H__
#define GUI_SLIDER_H__

#include <gui/ContainerView.h>
#include <gui/HorizontalPlacing.h>
#include <gui/NumberView.h>
#include <gui/TextView.h>
#include <pipeline/all.h>
#include "SliderImpl.h"

namespace gui {

class Slider : public pipeline::ProcessNode {

public:

	Slider(std::string name, double min, double max);

	Slider(std::string name, double min, double max, double value);

private:

	// a text view for the name of the slider
	boost::shared_ptr<TextView>   _textView;

	// the actual slider
	boost::shared_ptr<SliderImpl> _sliderImpl;

	// a view for the current value
	boost::shared_ptr<NumberView> _valueView;

	// a container view for the name, the slider, and the value
	boost::shared_ptr<ContainerView<HorizontalPlacing> > _container;
};

} // namespace gui

#endif // GUI_SLIDER_H__

