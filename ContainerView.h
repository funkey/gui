#ifndef GUI_CONTAINER_VIEW_H__
#define GUI_CONTAINER_VIEW_H__

#include <boost/concept_check.hpp>

#include <gui/IsPlacingStrategy.h>
#include <gui/ContainerPainter.h>
#include <gui/Signals.h>
#include <pipeline/all.h>
#include <signals/Slots.h>
#include <util/point.hpp>
#include <util/Logger.h>

namespace gui {

static logger::LogChannel containerviewlog("containerviewlog", "[ContainerView] ");

template <class PlacingStrategy>
class ContainerView : public pipeline::ProcessNode, public PlacingStrategy {

	BOOST_CONCEPT_ASSERT((IsPlacingStrategy<PlacingStrategy>));

public:

	ContainerView() {

		registerInputs(_painters, "painters");
		registerOutput(_container, "container");

		_painters.registerBackwardCallback(&ContainerView::onPainterAdded, this);
		_painters.registerBackwardCallback(&ContainerView::onPainterRemoved, this);
		_painters.registerBackwardCallback(&ContainerView::onModified, this);
		_painters.registerBackwardCallback(&ContainerView::onContentChanged, this);
		_painters.registerBackwardCallback(&ContainerView::onSizeChanged, this);

		_painters.registerBackwardSlot(_keyDown);
		_painters.registerBackwardSlot(_keyUp);
		_painters.registerBackwardSlot(_update);
		_painters.registerBackwardSlots(_mouseMoves);
		_painters.registerBackwardSlots(_mouseDowns);
		_painters.registerBackwardSlots(_mouseUps);

		_container.registerForwardCallback(&ContainerView::onUpdate, this);
		_container.registerForwardCallback(&ContainerView::onKeyDown, this);
		_container.registerForwardCallback(&ContainerView::onKeyUp, this);
		_container.registerForwardCallback(&ContainerView::onMouseMove, this);
		_container.registerForwardCallback(&ContainerView::onMouseDown, this);
		_container.registerForwardCallback(&ContainerView::onMouseUp, this);

		_container.registerForwardSlot(_modified);
		_container.registerForwardSlot(_contentChanged);
		_container.registerForwardSlot(_sizeChanged);
	}

	~ContainerView() {

		LOG_DEBUG(containerviewlog) << "destructed" << std::endl;
	}

private:

	void onPainterAdded(const pipeline::InputAdded<Painter>& signal) {

		LOG_ALL(containerviewlog) << "got a new painter " << typeName(*signal.getData()) << std::endl;

		if (!_container)
			_container.createData();

		_container->add(signal.getData());

		updateOffsets();

		_container->setOffsets(_offsets);

		_dirty = true;

		_contentChanged(ContentChanged());
		_sizeChanged(SizeChanged(_container->getSize()));
	}

	void onPainterRemoved(const pipeline::InputRemoved<Painter>& signal) {

		_container->remove(signal.getData());

		updateOffsets();

		_container->setOffsets(_offsets);

		_contentChanged(ContentChanged());
		_sizeChanged(SizeChanged(_container->getSize()));
	}

	void onModified(const pipeline::Modified& signal) {

		_dirty = true;

		_modified(signal);
	}

	void onContentChanged(const ContentChanged& signal) {

		_dirty = true;

		_contentChanged(signal);
	}

	void onSizeChanged(const SizeChanged& signal) {

		LOG_ALL(containerviewlog) << "got a SizeChanged signal -- recomputing my size" << std::endl;

		updateOffsets();

		_container->setOffsets(_offsets);

		_sizeChanged(SizeChanged(_container->getSize()));
	}

	void onUpdate(const pipeline::Update& signal) {

		LOG_ALL(containerviewlog) << "got an update signal" << std::endl;

		if (_dirty) {

			LOG_ALL(containerviewlog) << "I am dirty -- asking for updates" << std::endl;

			_update(signal);

			_dirty = false;
		}

		LOG_ALL(containerviewlog) << "I am up-to-date" << std::endl;
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

		LOG_ALL(containerviewlog) << "updating offsets of painters:" << std::endl;
		for (unsigned int i = 0; i < _painters.size(); i++)
			LOG_ALL(containerviewlog) << typeName(_painters[i]) << ": " << _painters[i]->getSize() << std::endl;

		_offsets = PlacingStrategy::getOffsets(_painters.begin(), _painters.end());
	}

	// input/output

	pipeline::Inputs<Painter>          _painters;
	pipeline::Output<ContainerPainter> _container;

	// backward signals

	signals::Slot<const pipeline::Update> _update;
	signals::Slot<const KeyDown>          _keyDown;
	signals::Slot<const KeyUp>            _keyUp;
	signals::Slots<const MouseMove>       _mouseMoves;
	signals::Slots<const MouseDown>       _mouseDowns;
	signals::Slots<const MouseUp>         _mouseUps;

	// forward signals

	signals::Slot<const pipeline::Modified>  _modified;
	signals::Slot<const ContentChanged>      _contentChanged;
	signals::Slot<const SizeChanged>         _sizeChanged;

	// the offsets of the painters in the container
	std::vector<util::point<double> > _offsets;

	// indicate that the content of the container changed
	bool _dirty;
};

} // namespace gui

#endif // GUI_CONTAINER_VIEW_H__

