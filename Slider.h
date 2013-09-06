#ifndef GUI_SLIDER_H__
#define GUI_SLIDER_H__

#include <boost/make_shared.hpp>

#include <gui/ContainerView.h>
#include <gui/HorizontalPlacing.h>
#include <gui/NumberView.h>
#include <gui/TextView.h>
#include <pipeline/all.h>
#include "SliderImpl.h"

namespace gui {

template <typename Precision>
class Slider : public pipeline::ProcessNode {

public:

	Slider(std::string name, Precision min, Precision max);

	Slider(std::string name, Precision min, Precision max, Precision value);

private:

	// a text view for the name of the slider
	boost::shared_ptr<TextView>   _textView;

	// the actual slider
	boost::shared_ptr<SliderImpl<Precision> > _sliderImpl;

	// a view for the current value
	boost::shared_ptr<NumberView<Precision> > _valueView;

	// a container view for the name, the slider, and the value
	boost::shared_ptr<ContainerView<HorizontalPlacing> > _container;
};

template <typename Precision>
Slider<Precision>::Slider(std::string name, Precision min, Precision max) :
	_textView(boost::make_shared<TextView>(name + ":")),
	_sliderImpl(boost::make_shared<SliderImpl<Precision> >(min, max, min)),
	_valueView(boost::make_shared<NumberView<Precision> >()),
	_container(boost::make_shared<ContainerView<HorizontalPlacing> >("slider")) {

	// setup internal pipeline connections
	_valueView->setInput(_sliderImpl->getOutput("value"));
	_container->addInput(_textView->getOutput());
	_container->addInput(_sliderImpl->getOutput("painter"));
	_container->addInput(_valueView->getOutput());

	registerOutput(_sliderImpl->getOutput("value"), "value");
	registerOutput(_container->getOutput(), "painter");
}

template <typename Precision>
Slider<Precision>::Slider(std::string name, Precision min, Precision max, Precision value) :
	_textView(boost::make_shared<TextView>(name + ":")),
	_sliderImpl(boost::make_shared<SliderImpl<Precision> >(min, max, value)),
	_valueView(boost::make_shared<NumberView<Precision> >()),
	_container(boost::make_shared<ContainerView<HorizontalPlacing> >("slider")) {

	// setup internal pipeline connections
	_valueView->setInput(_sliderImpl->getOutput("value"));
	_container->addInput(_textView->getOutput());
	_container->addInput(_sliderImpl->getOutput("painter"));
	_container->addInput(_valueView->getOutput());

	registerOutput(_sliderImpl->getOutput("value"), "value");
	registerOutput(_container->getOutput(), "painter");
}

} // namespace gui

#endif // GUI_SLIDER_H__

