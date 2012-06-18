#ifndef GUI_ROTATE_VIEW_H__
#define GUI_ROTATE_VIEW_H__

#include <pipeline.h>
#include <gui/RotatePainter.h>
#include <gui/Signals.h>
#include <gui/Keys.h>

namespace gui {

class RotateView : public pipeline::ProcessNode {

public:

	RotateView();

private:

	void onInputSet(const pipeline::InputSet<Painter>& signal);

	void onModified(const pipeline::Modified& signal);

	void onContentChanged(const ContentChanged& signal);

	void onSizeChanged(const SizeChanged& signal);

	void onUpdate(const pipeline::Update& signal);

	void onKeyUp(const KeyUp& signal);

	void onKeyDown(KeyDown& signal);

	void onMouseUp(const MouseUp& signal);

	void onMouseDown(MouseDown& signal);

	void onMouseMove(MouseMove& signal);

	util::point<double> unrotatePosition(const util::point<double>& position);

	void rotate(const util::point<double>& moved);

	// input/output

	pipeline::Input<Painter>        _content;
	pipeline::Output<RotatePainter> _rotated;

	// backward communications

	signals::Slot<const pipeline::Update> _update;
	signals::Slot<const KeyDown>          _keyDown;
	signals::Slot<const KeyUp>            _keyUp;
	signals::Slot<const MouseDown>        _mouseDown;
	signals::Slot<const MouseUp>          _mouseUp;
	signals::Slot<const MouseMove>        _mouseMove;

	// forward communications

	signals::Slot<const pipeline::Modified> _modified;
	signals::Slot<const ContentChanged>     _contentChanged;
	signals::Slot<const SizeChanged>        _sizeChanged;

	// the current rotation parameters
	double _x, _y, _z, _w;

	// the rotation parameters when the mouse was pressed
	double _prevX, _prevY, _prevZ, _prevW;

	// remember the last mouse position
	util::point<double> _buttonDown;

	// indicate that we are in dragging mode
	bool _dragging;
};

} // namespace gui

#endif // GUI_ROTATE_VIEW_H__

