#ifndef GUI_SWITCH_H__
#define GUI_SWITCH_H__

#include <string>

#include <gui/ContainerView.h>
#include <gui/HorizontalPlacing.h>
#include <gui/TextView.h>
#include <pipeline/all.h>
#include "SwitchImpl.h"

namespace gui {

class Switch : public pipeline::ProcessNode {

public:

	Switch(std::string name);

private:

	// a text view for the name of the switch
	boost::shared_ptr<TextView>   _textView;

	// the actual switch
	boost::shared_ptr<SwitchImpl> _switchImpl;

	// a container view for the name and the switch
	boost::shared_ptr<ContainerView<HorizontalPlacing> > _container;

};

} // namespace gui

#endif // GUI_SWITCH_H__

