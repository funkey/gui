#include <cmath>

#include <util/Logger.h>
#include "RotateView.h"

namespace gui {

static logger::LogChannel rotateviewlog("rotateviewlog", "[RotateView] ");

RotateView::RotateView() :
		_x(0.0),
		_y(1.0),
		_z(0.0),
		_w(0.0),
		_prevX(0.0),
		_prevY(1.0),
		_prevZ(0.0),
		_prevW(0.0),
		_buttonDown(0, 0),
		_dragging(false) {

	registerInput(_content, "painter");
	registerOutput(_rotated, "painter");

	_content.registerBackwardSlot(_update);
	_content.registerBackwardSlot(_keyDown);
	_content.registerBackwardSlot(_keyUp);
	_content.registerBackwardSlot(_mouseDown);
	_content.registerBackwardSlot(_mouseUp);
	_content.registerBackwardSlot(_mouseMove);
	_content.registerBackwardCallback(&RotateView::onInputSet, this);
	_content.registerBackwardCallback(&RotateView::onModified, this);
	_content.registerBackwardCallback(&RotateView::onContentChanged, this);
	_content.registerBackwardCallback(&RotateView::onSizeChanged, this);

	_rotated.registerForwardSlot(_modified);
	_rotated.registerForwardSlot(_contentChanged);
	_rotated.registerForwardSlot(_sizeChanged);
	_rotated.registerForwardCallback(&RotateView::onUpdate, this);
	_rotated.registerForwardCallback(&RotateView::onKeyUp, this);
	_rotated.registerForwardCallback(&RotateView::onKeyDown, this);
	_rotated.registerForwardCallback(&RotateView::onMouseUp, this);
	_rotated.registerForwardCallback(&RotateView::onMouseDown, this);
	_rotated.registerForwardCallback(&RotateView::onMouseMove, this);
}

void
RotateView::onInputSet(const pipeline::InputSet<Painter>& signal) {

	LOG_ALL(rotateviewlog) << "got a new painter" << std::endl;

	if (!_rotated)
		_rotated.createData();

	_rotated->setContent(_content);

	_modified();
}

void
RotateView::onModified(const pipeline::Modified& signal) {

	// just pass this signal on
	_modified();
}

void
RotateView::onContentChanged(const ContentChanged& signal) {

	_contentChanged();
}

void
RotateView::onSizeChanged(const SizeChanged& signal) {

	_rotated->updateSize();

	_sizeChanged(SizeChanged(_rotated->getSize()));
}

void
RotateView::onUpdate(const pipeline::Update& signal) {

	_update(signal);

	_rotated->setRotation(_x, _y, _z, _w/M_PI*180.0);
}

void
RotateView::onKeyUp(const KeyUp& signal) {

	// pass on the signal
	_keyUp(signal);
}

void
RotateView::onKeyDown(KeyDown& signal) {

	LOG_ALL(rotateviewlog) << "a key was pressed" << std::endl;

	if (signal.key == keys::R) {

		LOG_ALL(rotateviewlog) << "resetting rotation" << std::endl;

		_x = _z = _w = 0.0;
		_y = 1.0;

		_modified();

		signal.processed = true;

	} else {

		// pass on the signal
		_keyDown(signal);
	}
}

void
RotateView::onMouseUp(const MouseUp& signal) {

	LOG_ALL(rotateviewlog) << "a button was released" << std::endl;

	MouseUp unrotatedSignal = signal;

	unrotatedSignal.position = unrotatePosition(signal.position);

	_mouseUp(unrotatedSignal);
}

void
RotateView::onMouseDown(MouseDown& signal) {

	LOG_ALL(rotateviewlog) << "a button was pressed" << std::endl;

	MouseDown unrotatedSignal = signal;

	unrotatedSignal.position = unrotatePosition(signal.position);

	_mouseDown(unrotatedSignal);

	if (unrotatedSignal.processed)
		return;

	util::point<double> position = signal.position;

	LOG_ALL(rotateviewlog) << "mouse button " << signal.button << " down, position is " << position << std::endl;

	if (signal.button == buttons::Left && _rotated->getSize().contains(position)) {

		LOG_ALL(rotateviewlog) << "it's the left mouse button -- start dragging mode" << std::endl;

		_dragging = true;
		_buttonDown = position;

		_prevX = _x;
		_prevY = _y;
		_prevZ = _z;
		_prevW = _w;

		signal.processed = true;

		return;
	}
}

void
RotateView::onMouseMove(MouseMove& signal) {

	LOG_ALL(rotateviewlog) << "the mouse is moved" << std::endl;

	if (!signal.modifiers & buttons::LeftDown)
		_rotated->setHighlight(_rotated->getSize().contains(signal.position));

	MouseMove unrotatedSignal = signal;

	unrotatedSignal.position = unrotatePosition(signal.position);

	_mouseMove(unrotatedSignal);

	if (unrotatedSignal.processed)
		return;

	if (!_dragging) {

		return;
	}

	LOG_ALL(rotateviewlog) << "I am in dragging mode" << std::endl;

	double amp = 1.0;
	if (signal.modifiers & keys::ControlDown)
		amp = 10.0;

	// mouse is dragged
	if (signal.modifiers & buttons::LeftDown) {

		LOG_ALL(rotateviewlog) << "left button is still pressed" << std::endl;

		util::point<double> moved = signal.position - _buttonDown;

		//_buttonDown = signal.position;

		// change the current rotation according to mouse movement
		rotate(moved);

		_modified();

		signal.processed = true;

	} else {

		LOG_ALL(rotateviewlog) << "left button released -- stop dragging" << std::endl;

		_dragging = false;
	}
}

util::point<double>
RotateView::unrotatePosition(const util::point<double>& position) {

	// TODO:
	// • rotate (position.x, position.y, 0) around (_x, _y, _z) by -_w

	return position;
}

void
RotateView::rotate(const util::point<double>& moved) {

	LOG_ALL(rotateviewlog) << "current rotation: " << _w << ", (" << _x << ", " << _y << ", " << _z << ")" << std::endl;

	LOG_ALL(rotateviewlog) << "moved by: (" << moved.x << ", " << moved.y << ")" << std::endl;

	// previous rotation to quaternion
	double qx, qy, qz, qw;

	qx = _prevX*sin(_prevW/2.0);
	qy = _prevY*sin(_prevW/2.0);
	qz = _prevZ*sin(_prevW/2.0);
	qw =        cos(_prevW/2.0);

	// difference rotation as angle-axis
	double dx, dy, dz, dw;

	// ensure numerical stability
	double dnorm = sqrt(moved.y*moved.y + moved.x*moved.x);
	if (dnorm <= 0.0001)
		return;

	dx = -moved.y/dnorm;
	dy =  moved.x/dnorm;
	dz = 0.0;
	dw = dnorm/180.0*M_PI;

	LOG_ALL(rotateviewlog) << "add rotation: " << dw << ", (" << dx << ", " << dy << ", " << dz << ")" << std::endl;

	// difference rotation to quaternion
	double qdx, qdy, qdz, qdw;

	qdx = dx*sin(dw/2.0);
	qdy = dy*sin(dw/2.0);
	qdz = dz*sin(dw/2.0);
	qdw =    cos(dw/2.0);

	// composition of both quaternions
	double cx, cy, cz, cw;

	cw = (qdw*qw - qdx*qx - qdy*qy - qdz*qz);
	cx = (qdw*qx + qdx*qw + qdy*qz - qdz*qy);
	cy = (qdw*qy - qdx*qz + qdy*qw + qdz*qx);
	cz = (qdw*qz + qdx*qy - qdy*qx + qdz*qw);

	LOG_ALL(rotateviewlog) << "result quaternion: " << cw << ", (" << cx << ", " << cy << ", " << cz << ")" << std::endl;

	// back to axis-angle rotation
	_w = 2*acos(cw);
	_x = cx/sin(_w/2.0);
	_y = cy/sin(_w/2.0);
	_z = cz/sin(_w/2.0);

	// normalize rotation vector
	double norm = sqrt(_x*_x + _y*_y + _z*_z);
	_x = _x/norm;
	_y = _y/norm;
	_z = _z/norm;

	LOG_ALL(rotateviewlog) << "new rotation: " << _w << ", (" << _x << ", " << _y << ", " << _z << ")" << std::endl;
}

} // namespace gui
