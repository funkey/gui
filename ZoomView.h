#ifndef GUI_ZOOM_VIEW_H__
#define GUI_ZOOM_VIEW_H__

#include <pipeline/all.h>
#include <gui/ZoomPainter.h>
#include <gui/FingerSignals.h>
#include <gui/KeySignals.h>
#include <gui/Keys.h>
#include <gui/PointerSignalFilter.h>
#include <util/Logger.h>

namespace gui {

class ZoomView : public pipeline::SimpleProcessNode<>, public PointerSignalFilter {

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

	/**
	 * Filter callback for generic 2D input signals.
	 */
	bool filter(PointerSignal& signal);

	void onInputSet(const pipeline::InputSet<Painter>& signal);

	void onContentChanged(const ContentChanged& signal);

	void onSizeChanged(const SizeChanged& signal);

	void onResize(const Resize& signal);

	void onKeyUp(const KeyUp& signal);

	void onKeyDown(KeyDown& signal);

	void onPenMove(const PenMove& signal);

	void onPenIn(const PenIn& signal);

	void onPenOut(const PenOut& signal);

	void onFingerDown(const FingerDown& signal);

	void onFingerMove(const FingerMove& signal);

	void onFingerUp(const FingerUp& signal);

	double getFingerDistance();

	/**
	 * Returns true if there was recent pen activity.
	 */
	bool locked(unsigned long now, const util::point<double>& position);

	util::point<double> getFingerCenter();

	// input/output
	pipeline::Input<Painter>      _content;
	pipeline::Output<ZoomPainter> _zoomed;

	// backward communications
	signals::Slot<const KeyDown>          _keyDown;
	signals::Slot<const KeyUp>            _keyUp;

	// forward communications
	signals::Slot<const ContentChanged>      _contentChanged;
	signals::Slot<const SizeChanged>         _sizeChanged;

	// the speed of zooming
	double _zoomStep;

	// remember the last position of each finger
	std::map<int, FingerSignal> _fingerDown;

	// is the pen in the proximity of the screen?
	bool _penClose;

	// the last known position of the pen
	util::point<double> _lastPen;

	// automatically scale content to fit the desired size (which can be given 
	// on construction or set via a resize signal)
	bool _autoscale;
};

} // namespace gui

#endif // GUI_ZOOM_VIEW_H__

