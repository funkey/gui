#ifndef GUI_NAMED_VIEW_H__
#define GUI_NAMED_VIEW_H__

#include <boost/shared_ptr.hpp>

#include <pipeline/all.h>
#include <gui/ContainerView.h>
#include <gui/VerticalPlacing.h>
#include <gui/Painter.h>
#include <gui/TextView.h>

namespace gui {

class NamedView : public pipeline::SimpleProcessNode {

public:

	NamedView(const std::string& name);

private:

	void updateOutputs();

	void onInputSet(const pipeline::InputSetBase& signal);

	pipeline::Input<Painter> _content;

	boost::shared_ptr<TextView> _textView;

	boost::shared_ptr<ContainerView<VerticalPlacing> > _container;

	bool _contentAdded;
};

} // namespace gui

#endif // GUI_NAMED_VIEW_H__

