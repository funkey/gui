#include <boost/make_shared.hpp>

#include "NamedView.h"

namespace gui {

logger::LogChannel namedviewlog("namedviewlog", "[NamedView] ");

NamedView::NamedView(const std::string& name) :
	_textView(new TextView(name)),
	_container(new ContainerView<VerticalPlacing>(name)),
	_contentAdded(false) {

	registerInput(_content, "painter");
	registerOutput(_container->getOutput(), "painter");

	_content.registerCallback(&NamedView::onInputSet, this);
}

void
NamedView::updateOutputs() {

	LOG_DEBUG(namedviewlog) << "update called" << std::endl;
}

void
NamedView::onInputSet(const pipeline::InputSetBase& /*signal*/) {

	LOG_DEBUG(namedviewlog) << "content was set, updating container" << std::endl;

	_container->clearInputs(0);

	_container->addInput(_textView->getOutput());
	_container->addInput(_content);
}

} // namespace gui
