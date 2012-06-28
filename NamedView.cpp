#include <boost/make_shared.hpp>

#include "NamedView.h"

namespace gui {

logger::LogChannel namedviewlog("namedviewlog", "[NamedView] ");

NamedView::NamedView(const std::string& name) :
	_textView(boost::make_shared<TextView>(name)),
	_container(boost::make_shared<ContainerView<VerticalPlacing> >()),
	_contentAdded(false) {

	registerInput(_content, "painter");
	registerOutput(_container->getOutput(), "painter");

	_content.registerBackwardCallback(&NamedView::onInputSet, this);
}

void
NamedView::onInputSet(const pipeline::InputSetBase& signal) {

	LOG_DEBUG(namedviewlog) << "content was set, updating container" << std::endl;

	_container->clearInputs(0);

	_container->addInput(_textView->getOutput());
	_container->addInput(_content);
}

void
NamedView::updateOutputs() {

	// nothing to do here
}

} // namespace gui
