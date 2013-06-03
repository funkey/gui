#ifndef GUI_CONTAINER_VIEW_H__
#define GUI_CONTAINER_VIEW_H__

#include <boost/concept_check.hpp>

#include <gui/IsPlacingStrategy.h>
#include <gui/ContainerPainter.h>
#include <gui/MouseSignals.h>
#include <gui/KeySignals.h>
#include <pipeline/all.h>
#include <signals/Slots.h>
#include <util/point.hpp>
#include <util/Logger.h>

namespace gui {

static logger::LogChannel containerviewlog("containerviewlog", "[ContainerView] ");

template <class PlacingStrategy>
class ContainerView : public pipeline::SimpleProcessNode<>, public PlacingStrategy {

	BOOST_CONCEPT_ASSERT((IsPlacingStrategy<PlacingStrategy>));

public:

	ContainerView(std::string name = "") :
			SimpleProcessNode<>(name) {

		registerInputs(_painters, "painters");
		registerOutput(_container, "container");

		_painters.registerBackwardCallback(&ContainerView::onPainterAdded, this);
		_painters.registerBackwardCallback(&ContainerView::onPainterRemoved, this);
		_painters.registerBackwardCallback(&ContainerView::onPaintersCleared, this);
		_painters.registerBackwardCallback(&ContainerView::onContentChanged, this);
		_painters.registerBackwardCallback(&ContainerView::onSizeChanged, this);

		_painters.registerBackwardSlot(_keyDown);
		_painters.registerBackwardSlot(_keyUp);
		_painters.registerBackwardSlots(_mouseMoves);
		_painters.registerBackwardSlots(_mouseDowns);
		_painters.registerBackwardSlots(_mouseUps);

		_container.registerForwardCallback(&ContainerView::onKeyDown, this);
		_container.registerForwardCallback(&ContainerView::onKeyUp, this);
		_container.registerForwardCallback(&ContainerView::onMouseMove, this);
		_container.registerForwardCallback(&ContainerView::onMouseDown, this);
		_container.registerForwardCallback(&ContainerView::onMouseUp, this);

		_container.registerForwardSlot(_contentChanged);
		_container.registerForwardSlot(_sizeChanged);
	}

private:

	void updateOutputs() {

		updateOffsets();
		_container->setOffsets(_offsets);
	}

	void onPainterAdded(const pipeline::InputAdded<Painter>& signal) {

		LOG_ALL(containerviewlog) << getName() << ": " << "got a new painter " << typeName(*signal.getData()) << std::endl;

		if (!_container)
			_container.createData();

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

		setDirty(_container);

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

	void onMouseMove(MouseMove& signal) {

		for (unsigned int i = 0; i < _mouseMoves.size(); i++) {

			MouseMove offsetSignal = signal;

			offsetSignal.position -= _offsets[i];

			_mouseMoves[i](offsetSignal);

			if (offsetSignal.processed) {

				signal.processed = true;
				break;
			}
		}
	}

	void onMouseDown(MouseDown& signal) {

		for (unsigned int i = 0; i < _mouseDowns.size(); i++) {

			MouseDown offsetSignal = signal;

			offsetSignal.position -= _offsets[i];

			// mouse down events are only forwarded to painters under the cursor
			if (!_painters[i]->getSize().contains(offsetSignal.position))
				continue;

			_mouseDowns[i](offsetSignal);

			if (offsetSignal.processed) {

				signal.processed = true;
				break;
			}
		}
	}

	void onMouseUp(MouseUp& signal) {

		for (unsigned int i = 0; i < _mouseUps.size(); i++) {

			MouseUp offsetSignal = signal;

			offsetSignal.position -= _offsets[i];

			_mouseUps[i](offsetSignal);

			if (offsetSignal.processed) {

				signal.processed = true;
				break;
			}
		}
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
	signals::Slots<const MouseMove>       _mouseMoves;
	signals::Slots<const MouseDown>       _mouseDowns;
	signals::Slots<const MouseUp>         _mouseUps;

	// forward signals

	signals::Slot<const ContentChanged>      _contentChanged;
	signals::Slot<const SizeChanged>         _sizeChanged;

	// the offsets of the painters in the container
	std::vector<util::point<double> > _offsets;
};

} // namespace gui

#endif // GUI_CONTAINER_VIEW_H__

