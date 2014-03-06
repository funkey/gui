#ifndef GUI_CONTAINER_VIEW_H__
#define GUI_CONTAINER_VIEW_H__

#include <boost/concept_check.hpp>

#include <gui/IsPlacingStrategy.h>
#include <gui/ContainerPainter.h>
#include <gui/MouseSignals.h>
#include <gui/KeySignals.h>
#include <gui/PointerSignalFilter.h>
#include <gui/WindowSignalFilter.h>
#include <pipeline/all.h>
#include <signals/Slots.h>
#include <util/point.hpp>
#include <util/Logger.h>

namespace gui {

static logger::LogChannel containerviewlog("containerviewlog", "[ContainerView] ");

template <class PlacingStrategy>
class ContainerView : public pipeline::SimpleProcessNode<>, public PointerSignalFilter, public WindowSignalFilter, public PlacingStrategy {

	BOOST_CONCEPT_ASSERT((IsPlacingStrategy<PlacingStrategy>));

public:

	ContainerView(std::string name = "") :
			SimpleProcessNode<>(name) {

		registerInputs(_painters, "painters");
		registerOutput(_container, "container");

		// establish pointer signal filter
		PointerSignalFilter::filterBackward(_container, _painters, this);

		// establish window signal filter
		WindowSignalFilter::filterForward(_painters, _container, this);

		_painters.registerBackwardCallback(&ContainerView::onPainterAdded, this);
		_painters.registerBackwardCallback(&ContainerView::onPainterRemoved, this);
		_painters.registerBackwardCallback(&ContainerView::onPaintersCleared, this);
		_painters.registerBackwardCallback(&ContainerView::onContentChanged, this);
		_painters.registerBackwardCallback(&ContainerView::onSizeChanged, this);

		_painters.registerBackwardSlot(_keyDown);
		_painters.registerBackwardSlot(_keyUp);

		_container.registerForwardCallback(&ContainerView::onKeyDown, this);
		_container.registerForwardCallback(&ContainerView::onKeyUp, this);

		_container.registerForwardSlot(_contentChanged);
		_container.registerForwardSlot(_sizeChanged);
	}

private:

	void updateOutputs() {

		updateOffsets();
		_container->setOffsets(_offsets);
	}

	bool filter(PointerSignal& signal, unsigned int i) {

		// It can happen that we receive events to filter before we updated our 
		// outputs and therefore don't know the offsets, yet. In this cases, 
		// ignore the event.
		if (i >= _offsets.size())
			return false;

		signal.position -= _offsets[i];

		return true;
	}

	void onPainterAdded(const pipeline::InputAdded<Painter>& signal) {

		LOG_ALL(containerviewlog) << getName() << ": got a new painter " << typeName(*signal.getData()) << std::endl;

		if (!_container)
			_container = new gui::ContainerPainter();

		_container->add(signal.getData());

		setDirty(_container);

		_contentChanged(ContentChanged());
		_sizeChanged(SizeChanged(_container->getSize()));
	}

	void onPainterRemoved(const pipeline::InputRemoved<Painter>& signal) {

		LOG_ALL(containerviewlog) << getName() << ": " << "painter removed " << typeName(*signal.getData()) << std::endl;

		_container->remove(signal.getData());

		setDirty(_container);

		_contentChanged(ContentChanged());
		_sizeChanged(SizeChanged(_container->getSize()));
	}

	void onPaintersCleared(const pipeline::InputsCleared&) {

		LOG_ALL(containerviewlog) << getName() << ": " << "painters cleared" << std::endl;

		_container->clear();

		setDirty(_container);

		_contentChanged(ContentChanged());
		_sizeChanged(SizeChanged(_container->getSize()));
	}

	void onContentChanged(const ContentChanged& signal) {

		LOG_ALL(containerviewlog) << getName() << ": " << "got a ContentChanged signal -- passing it on" << std::endl;

		_contentChanged(signal);
	}

	void onSizeChanged(const SizeChanged&) {

		LOG_ALL(containerviewlog) << getName() << ": " << "got a SizeChanged signal -- recomputing my size" << std::endl;

		_container->updateSize();
		setDirty(_container);

		_sizeChanged(SizeChanged(_container->getSize()));
	}

	void onKeyDown(const KeyDown& signal) {

		_keyDown(signal);
	}

	void onKeyUp(const KeyUp& signal) {

		_keyUp(signal);
	}

	void updateOffsets() {

		LOG_ALL(containerviewlog) << getName() << ": " << "updating offsets of painters:" << std::endl;
		for (unsigned int i = 0; i < _painters.size(); i++)
			LOG_ALL(containerviewlog) << getName() << ": " << typeName(_painters[i]) << ": " << _painters[i]->getSize() << std::endl;

		_offsets = PlacingStrategy::getOffsets(_painters.begin(), _painters.end());
	}

	// input/output

	pipeline::Inputs<Painter>          _painters;
	pipeline::Output<ContainerPainter> _container;

	// backward signals

	signals::Slot<const KeyDown>          _keyDown;
	signals::Slot<const KeyUp>            _keyUp;

	// forward signals

	signals::Slot<const ContentChanged>      _contentChanged;
	signals::Slot<const SizeChanged>         _sizeChanged;

	// the offsets of the painters in the container
	std::vector<util::point<double> > _offsets;
};

} // namespace gui

#endif // GUI_CONTAINER_VIEW_H__

