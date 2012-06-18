#include <boost/make_shared.hpp>

#include "Slider.h"

namespace gui {

Slider::Slider(std::string name, double min, double max) :
	_textView(boost::make_shared<TextView>(name + ":")),
	_sliderImpl(boost::make_shared<SliderImpl>(min, max, min)),
	_valueView(boost::make_shared<NumberView>()),
	_container(boost::make_shared<ContainerView<HorizontalPlacing> >()) {

	// setup internal pipeline connections
	_valueView->setInput(_sliderImpl->getOutput("value"));
	_container->addInput(_textView->getOutput());
	_container->addInput(_sliderImpl->getOutput("painter"));
	_container->addInput(_valueView->getOutput());

	registerOutput(_sliderImpl->getOutput("value"), "value");
	registerOutput(_container->getOutput(), "painter");
}

Slider::Slider(std::string name, double min, double max, double value) :
	_textView(boost::make_shared<TextView>(name + ":")),
	_sliderImpl(boost::make_shared<SliderImpl>(min, max, value)),
	_valueView(boost::make_shared<NumberView>()),
	_container(boost::make_shared<ContainerView<HorizontalPlacing> >()) {

	// setup internal pipeline connections
	_valueView->setInput(_sliderImpl->getOutput("value"));
	_container->addInput(_textView->getOutput());
	_container->addInput(_sliderImpl->getOutput("painter"));
	_container->addInput(_valueView->getOutput());

	registerOutput(_sliderImpl->getOutput("value"), "value");
	registerOutput(_container->getOutput(), "painter");
}

} // namespace gui
