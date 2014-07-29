#ifndef GUI_ROTATE_VIEW_H__
#define GUI_ROTATE_VIEW_H__

#include <pipeline/SimpleProcessNode.h>
#include <gui/RotatePainter.h>
#include <gui/MouseSignals.h>
#include <gui/KeySignals.h>
#include <gui/Keys.h>

namespace gui {

class RotateView : public pipeline::SimpleProcessNode<> {

public:

	RotateView();

private:

	void updateOutputs();

	void onModified(const pipeline::Modified& signal);

	void onSizeChanged(const SizeChanged& signal);

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

	signals::Slot<const KeyDown>          _keyDown;
	signals::Slot<const KeyUp>            _keyUp;
	signals::Slot<const MouseDown>        _mouseDown;
	signals::Slot<const MouseUp>          _mouseUp;
	signals::Slot<const MouseMove>        _mouseMove;

	// forward communications

	signals::Slot<const SizeChanged>        _sizeChanged;

	// the current rotation parameters
	double _x, _y, _z, _w;

	// the rotation parameters when the mouse was pressed
	double _prevX, _prevY, _prevZ, _prevW;

	// remember the last mouse position
	util::point<double> _buttonDown;

	// indicate that we are in dragging mode
	bool _dragging;

	// indicate that the content changed and needs update
	bool _contentChanged;
};

} // namespace gui

#endif // GUI_ROTATE_VIEW_H__

