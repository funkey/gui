#include "ImageView.h"

namespace gui {

ImageView::ImageView() :
	_dirty(true) {

	registerInput(_image, "image");
	registerOutput(_painter, "painter");

	_image.registerBackwardSlot(_update);

	_image.registerBackwardCallback(&ImageView::onInputImageSet, this);
	_image.registerBackwardCallback(&ImageView::onModified, this);
	_image.registerBackwardCallback(&ImageView::onUpdated, this);

	_painter.registerForwardSlot(_modified);
	_painter.registerForwardSlot(_contentChanged);
	_painter.registerForwardSlot(_sizeChanged);

	_painter.registerForwardCallback(&ImageView::onUpdate, this);
}

void
ImageView::onInputImageSet(const pipeline::InputSet<Image>& signal) {

	if (!_painter)
		_painter.createData();

	_painter->setImage(_image);

	LOG_ALL(imageviewlog) << "got a new input image -- sending SizeChanged" << std::endl;
	LOG_ALL(imageviewlog) << "image has size: " << _painter->getSize() << std::endl;

	_sizeChanged();
}

void
ImageView::onModified(const pipeline::Modified& signal) {

	LOG_ALL(imageviewlog) << "the image changed" << std::endl;

	_modified();

	_dirty = true;
}

void
ImageView::onUpdated(const pipeline::Updated& signal) {

	LOG_ALL(imageviewlog) << "the image was updated" << std::endl;

	LOG_ALL(imageviewlog) << "updating my painter" << std::endl;

	util::rect<double> oldSize = _painter->getSize();

	LOG_ALL(imageviewlog) << "old size is " << oldSize << std::endl;

	// explicitly update the painter
	_painter->update();

	util::rect<double> newSize = _painter->getSize();

	LOG_ALL(imageviewlog) << "new size is " << newSize << std::endl;

	if (oldSize == newSize) {

		LOG_ALL(imageviewlog) << "image size did not change -- sending ContentChanged" << std::endl;

		_contentChanged();

	} else {

		LOG_ALL(imageviewlog) << "image size did change -- sending SizeChanged" << std::endl;

		_sizeChanged();
	}

	_dirty = false;
}

void
ImageView::onUpdate(const pipeline::Update& signal) {

	LOG_ALL(imageviewlog) << "got an update notification" << std::endl;

	if (_dirty) {

		LOG_ALL(imageviewlog) << "I am dirty, asking for updates..." << std::endl;

		// backward the update signal to the image
		_update(signal);
	}

	LOG_ALL(imageviewlog) << "I am up-to-date" << std::endl;
}

} // namespace gui
