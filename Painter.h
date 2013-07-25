#ifndef GUI_PAINTER_H__
#define GUI_PAINTER_H__

#include <pipeline/Data.h>
#include <util/rect.hpp>
#include <util/point.hpp>

namespace gui {

class Painter : public pipeline::Data {

public:

	/**
	 * Default constructor.
	 */
	Painter(std::string name = "") :
		_size(0, 0, 1, 1),
		_name(name) {}

	/**
	 * Destructor.
	 */
	virtual ~Painter() {};

	/**
	 * Draw this painter.
	 *
	 * Has to be overwritten in subclasses to create new painters. The arguments
	 * can safely be ignored, however, they might be helpful speed up drawing.  
	 * The painters can return true to initiate a redraw as soon as possible.  
	 * This can be used for animations. Otherwise, the painter gets redrawn only 
	 * if the upstream process node initiates a redraw (which can happen because 
	 * of a window mapping event or the setting of a dirty flag from downstream 
	 * process nodes).
	 *
	 * @param roi        The region that is requested.
	 * @param resolution The resolution in pixels per roi-unit.
	 * @return True, if the painter wants to be called again as soon as 
	 * possible.
	 */
	virtual bool draw(
			const util::rect<double>&  roi,
			const util::point<double>& resolution) = 0;

	/**
	 * Get the size of this painter.
	 *
	 * @return The size of this painter.
	 */
	const util::rect<double>& getSize() const { return _size; }

	const std::string& getName() const { return _name; }

protected:

	/**
	 * Change the reported size of this painter.
	 */
	void setSize(double minX, double minY, double maxX, double maxY) {

		_size.minX = minX;
		_size.minY = minY;
		_size.maxX = maxX;
		_size.maxY = maxY;
	}

	/**
	 * Change the reported size of this painter.
	 */
	void setSize(const util::rect<double>& size) {

		_size = size;
	}

private:

	// the size of this painter
	util::rect<double> _size;

	std::string _name;
};

} // namespace gui

#endif // GUI_PAINTER_H__

