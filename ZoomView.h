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

	ZoomView();

private:

	void updateOutputs();

	void onInputSet(const pipeline::InputSet<Painter>& signal);

	void onContentChanged(const ContentChanged& signal);

	void onSizeChanged(const SizeChanged& signal);

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

	// the scale of the zoomed content
	double _scale;

	// the shift of the zoomed content
	util::point<double> _shift;

	// the speed of zooming
	double _zoomStep;

	// remember the last mouse position
	util::point<double> _buttonDown;

	// indicate that we are in dragging mode
	bool _dragging;
};

} // namespace gui

#endif // GUI_ZOOM_VIEW_H__

