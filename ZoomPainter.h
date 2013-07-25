#ifndef GUI_ZOOM_PAINTER_H__
#define GUI_ZOOM_PAINTER_H__

#include <boost/shared_ptr.hpp>

#include <gui/Painter.h>
#include <util/point.hpp>
#include <util/rect.hpp>

namespace gui {

class ZoomPainter : public Painter {

public:

	ZoomPainter();

	/**
	 * Set the content of this painter.
	 */
	void setContent(boost::shared_ptr<Painter> content);

	/**
	 * Enable or disable autoscaling.
	 * 
	 * When enabled, the content of this painter will be rescaled and shifted, 
	 * such that they fill the desired size (see setDesiredSize()) when scale 
	 * and shift are set to 1 and (0, 0), respectively. Zooming and panning can 
	 * still be used as usual.
	 */
	void setAutoscale(bool autoscale = true) { _autoscale = autoscale; }

	/**
	 * Set the desired size of this painter. Has an effect only if 
	 * setAutoscale() has been set to true.
	 */
	void setDesiredSize(const util::rect<double>& desiredSize) { _desiredSize = desiredSize; updateScaleAndShift(); }

	/**
	 * Zoom onto the given point. The current scale will be multiplied by 
	 * 'zoomChange'.
	 */
	void zoom(double zoomChange, const util::point<double>& anchor);

	/**
	 * Drag the content in the given direction.
	 */
	void drag(const util::point<double>& direction);

	/**
	 * Set the user requested scale explicitly.
	 */
	void setUserScale(double scale);

	/**
	 * Set the user requested shift explicitly.
	 */
	void setUserShift(const util::point<double>& shift);

	/**
	 * Reset the user requested scale and shift.
	 */
	void reset();

	/**
	 * Get the current effective scale. Accounts for automatic scaling.
	 */
	double getScale() { return _scale; }

	/**
	 * Get the current effective shift. Accounts for automatic scaling.
	 */
	const util::point<double>& getShift() { return _shift; }

	/**
	 * Inherited from Painter.
	 */
	bool draw(const util::rect<double>& roi, const util::point<double>& resolution);

	/**
	 * Apply the inverse zoom- and scale-transformation to a given point.
	 */
	util::point<double> invert(const util::point<double>& point);

	/**
	 * Recalculate scale and shift.
	 */
	void updateScaleAndShift();

private:

	// the scale and shift of this painter as requested by the user
	double _userScale;
	util::point<double> _userShift;

	// the scale and shift of this painter to fit the content to the desired 
	// size
	double _autoScale;
	util::point<double> _autoShift;

	// the effective scale and shift, taking into account possible autoscaling
	double _scale;
	util::point<double> _shift;

	// the content to scale and shift
	boost::shared_ptr<Painter> _content;

	// automatically scale content to fit to the desired size
	bool _autoscale;

	// the desired size of this painter
	util::rect<double> _desiredSize;
};

} // namespace gui

#endif // GUI_ZOOM_PAINTER_H__

