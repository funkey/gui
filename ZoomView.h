#ifndef GUI_ZOOM_VIEW_H__
#define GUI_ZOOM_VIEW_H__

#include <pipeline/all.h>
#include <gui/ZoomPainter.h>
#include <gui/Signals.h>
#include <gui/Keys.h>
#include <util/Logger.h>

namespace gui {

class ZoomView : public pipeline::SimpleProcessNode<> {

public:

	/**
	 * Default constructor.
	 *
	 * @param autoscale
	 *             If set to true, this view will automatically scale the 
	 *             content to fit the size this view was requested to have. For 
	 *             that, a Resize signal has to be sent to this view.
	 */
	ZoomView(bool autoscale = false);

	/**
	 * Create a zoom view of a fixed size. The content will be scaled and 
	 * shifted to fit into the desired size. Zooming and panning changes the 
	 * area of the content that is fit into the desired size.
	 */
	ZoomView(const util::rect<double>& desiredSize);

private:

	void updateOutputs();

	void onInputSet(const pipeline::InputSet<Painter>& signal);

	void onContentChanged(const ContentChanged& signal);

	void onSizeChanged(const SizeChanged& signal);

	void onResize(const Resize& signal);

	void onKeyUp(const KeyUp& signal);

	void onKeyDown(KeyDown& signal);

	void onMouseUp(const MouseUp& signal);

	void onMouseDown(const MouseDown& signal);

	void onMouseMove(const MouseMove& signal);

	// input/output
	pipeline::Input<Painter>      _content;
	pipeline::Output<ZoomPainter> _zoomed;

	// backward communications
	signals::Slot<const KeyDown>          _keyDown;
	signals::Slot<const KeyUp>            _keyUp;
	signals::Slot<const MouseDown>        _mouseDown;
	signals::Slot<const MouseUp>          _mouseUp;
	signals::Slot<const MouseMove>        _mouseMove;

	// forward communications
	signals::Slot<const ContentChanged>      _contentChanged;
	signals::Slot<const SizeChanged>         _sizeChanged;

	// the speed of zooming
	double _zoomStep;

	// remember the last mouse position
	util::point<double> _buttonDown;

	// indicate that we are in dragging mode
	bool _dragging;

	// automatically scale content to fit the desired size (which can be given 
	// on construction or set via a resize signal)
	bool _autoscale;
};

} // namespace gui

#endif // GUI_ZOOM_VIEW_H__

