#ifndef IMAGE_VIEW_H__
#define IMAGE_VIEW_H__

#include <pipeline/ProcessNode.h>
#include <pipeline/signals/Modified.h>
#include <pipeline/signals/Update.h>
#include <pipeline/signals/Updated.h>
#include <gui/Signals.h>
#include <util/Logger.h>
#include <imageprocessing/Image.h>
#include "ImagePainter.h"

namespace gui {

static logger::LogChannel imageviewlog("imageviewlog", "[ImageView] ");

class ImageView : public pipeline::ProcessNode {

public:

	ImageView();

private:

	void onInputImageSet(const pipeline::InputSet<Image>& signal);

	void onModified(const pipeline::Modified& signal);

	void onUpdate(const pipeline::Update& signal);

	void onUpdated(const pipeline::Updated& signal);

	// input/output

	pipeline::Input<Image>                      _image;
	pipeline::Output<gui::ImagePainter<Image> > _painter;

	// backward signals

	signals::Slot<const pipeline::Update> _update;

	// forward signals

	signals::Slot<const pipeline::Modified>  _modified;
	signals::Slot<const gui::SizeChanged>    _sizeChanged;
	signals::Slot<const gui::ContentChanged> _contentChanged;

	// indicates changes in the image

	bool _dirty;
};

} // namespace gui

#endif // IMAGE_VIEW_H__

