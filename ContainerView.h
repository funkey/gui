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
			SimpleProcessNode<>(name),
			_container(new ContainerPainter()) {

		registerInputs(_painters, "painters");
		registerOutput(_container, "container");

		// establish pointer signal filter
		PointerSignalFilter::filterBackward(_container, _painters, this);

		// establish window signal filter
		WindowSignalFilter::filterForward(_painters, _container, this);

		_painters.registerCallback(&ContainerView::onPainterAdded, this);
		_painters.registerCallback(&ContainerView::onPainterRemoved, this);
		_painters.registerCallback(&ContainerView::onPaintersCleared, this);
		_painters.registerCallback(&ContainerView::onContentChanged, this);
		_painters.registerCallback(&ContainerView::onSizeChanged, this);

		_painters.registerSlot(_keyDown);
		_painters.registerSlot(_keyUp);

		_container.registerCallback(&ContainerView::onKeyDown, this);
		_container.registerCallback(&ContainerView::onKeyUp, this);

		_container.registerSlot(_contentChanged);
		_container.registerSlot(_sizeChanged);
	}

private:

	void updateOutputs() {

		updateSetPainters();
		updateOffsets();
		updatePainter();

		_sizeChanged(SizeChanged(_container->getSize()));
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

	void onPainterAdded(const pipeline::InputAdded<Painter>&) {

		LOG_ALL(containerviewlog) << getName() << ": got a new painter" << std::endl;

		setDirty(_container);
	}

	void onPainterRemoved(const pipeline::InputRemoved<Painter>&) {

		LOG_ALL(containerviewlog) << getName() << ": " << "painter removed" << std::endl;

		setDirty(_container);
	}

	void onPaintersCleared(const pipeline::InputsCleared&) {

		LOG_ALL(containerviewlog) << getName() << ": " << "painters cleared" << std::endl;

		setDirty(_container);
	}

	void onContentChanged(const ContentChanged& signal) {

		LOG_ALL(containerviewlog) << getName() << ": " << "got a ContentChanged signal -- passing it on" << std::endl;

		_contentChanged(signal);
	}

	void onSizeChanged(const SizeChanged&) {

		LOG_ALL(containerviewlog) << getName() << ": " << "got a SizeChanged signal -- recomputing my size" << std::endl;

		_container->updateSize();
		_sizeChanged(SizeChanged(_container->getSize()));
	}

	void onKeyDown(const KeyDown& signal) {

		_keyDown(signal);
	}

	void onKeyUp(const KeyUp& signal) {

		_keyUp(signal);
	}

	/**
	 * From all the painter inputs, collect the ones that are really set to 
	 * valid painters.
	 */
	void updateSetPainters() {

		_setPainters.clear();
		for (unsigned int i = 0; i < _painters.size(); i++)
			if (_painters[i].isSet()) {

				LOG_ALL(containerviewlog) << getName() << ": " << typeName(_painters[i]) << ": " << _painters[i]->getSize() << std::endl;
				_setPainters.push_back(_painters[i].getSharedPointer());
			}
	}

	/**
	 * According to the placing strategy, update the offsets of the set 
	 * painters.
	 */
	void updateOffsets() {

		LOG_ALL(containerviewlog) << getName() << ": " << "updating offsets of painters:" << std::endl;

		_offsets = PlacingStrategy::getOffsets(_setPainters.begin(), _setPainters.end());
	}

	/**
	 * Add the set painters with their offsets to the output painter.
	 */
	void updatePainter() {

		_container->clear();

		assert(_setPainters.size() == _offsets.size());

		for (unsigned int i = 0; i < _setPainters.size(); i++)
			_container->add(_setPainters[i], _offsets[i]);
	}

	// input/output

	pipeline::Inputs<Painter>          _painters;
	pipeline::Output<ContainerPainter> _container;

	// backward signals

	signals::Slot<const KeyDown> _keyDown;
	signals::Slot<const KeyUp>   _keyUp;

	// forward signals

	signals::Slot<const ContentChanged> _contentChanged;
	signals::Slot<const SizeChanged>    _sizeChanged;

	// the set painters and their offsets in the container
	std::vector<boost::shared_ptr<Painter> > _setPainters;
	std::vector<util::point<double> >        _offsets;
};

} // namespace gui

#endif // GUI_CONTAINER_VIEW_H__

