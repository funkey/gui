#ifndef GUI_GRID_PLAING_H__
#define GUI_GRID_PLAING_H__

#include <cmath>

#include <gui/OpenGl.h>
#include <util/rect.hpp>
#include <util/point.hpp>
#include <util/Logger.h>

using namespace logger;

static LogChannel gridplacinglog("gridplacinglog");

namespace gui {

/**
 * A placing strategy that places views on a quadratic grid.
 */
class GridPlacing {

public:

	/**
	 * Used to set the alignment of the content.
	 */
	enum Align {

		TopLeft,
		TopRight,
		Centered,
		BottomLeft,
		BottomRight
	};

	/**
	 * Create a new vertical placing strategy.
	 *
	 * @param spacing The space between two views.
	 * @param align <code>Left</code>, <code>Centered</code>, or
	 *              <code>Right</code>: the horizontal alignement of
	 *              the views.
	 */
	GridPlacing(double spacing = 0, Align align = Centered) :
		_spacing(spacing),
		_align(align) {}

	/**
	 * Set the spacing for this placing strategy.
	 *
	 * @param spacing The space between two views.
	 */
	void setSpacing(double spacing) { _spacing = spacing; }

	/**
	 * Set the horizontal alignment for this placing strategy.
	 *
	 * @param align <code>Left</code>, <code>Centered</code>, or
	 *              <code>Right</code>: the horizontal alignement of
	 *              the views.
	 */
	void setAlign(Align align) { _align = align; }

	/**
	 * Draw views.
	 *
	 * @param begin Begin iterator to collection of views.
	 * @param end End ViewIterator to collection of views.
	 * @param roi The requested region to draw to.
	 * @param resolution The resolution in pixels of one OpenGl unit.
	 */
	template <typename ViewIterator>
	void draw(
			const ViewIterator&  begin,
			const ViewIterator&  end,
			const rect<double>&  roi,
			const point<double>& resolution);

	/**
	 * Compute the resulting size of drawing all views.
	 *
	 * @param begin Begin iterator to collection of views.
	 * @param end End ViewIterator to collection of views.
	 */
	template <typename ViewIterator>
	rect<double> computeSize(
			const ViewIterator& begin,
			const ViewIterator& end);

private:

	// the space between two views
	double _spacing;

	// how to align the views horizontally
	Align  _align;

	// the size of this placing
	rect<double> _size;

	// the column width (maximal width of all views)
	double _columnWidth;

	// the row height (maximal height of all views)
	double _rowHeight;

	// the number of columns
	unsigned int _columns;

	// the number of rows
	unsigned int _rows;
};

/*****************
 * IMPLEMENTAION *
 *****************/

template <typename ViewIterator>
rect<double>
GridPlacing::computeSize(const ViewIterator& begin, const ViewIterator& end) {

	_size = rect<double>(0, 0, 0, 0);

	int numViews = end - begin;

	if (numViews == 0)
		return _size;

	// determine the number of columns and rows
	_columns = ceil(sqrt((double)numViews));
	_rows    = numViews/_columns + (numViews%_columns == 0 ? 0 : 1);

	LOG_ALL(gridplacinglog) << "[GridPlacing] determining size for " << (end - begin) << " views" << std::endl;
	LOG_ALL(gridplacinglog) << "[GridPlacing] will create a grid with " << _columns << " columns" << std::endl;

	_columnWidth = 0;
	_rowHeight   = 0;
	for (ViewIterator view = begin; view != end; view++) {

		_columnWidth = std::max(_columnWidth, (*view)->getSize().width());
		_rowHeight   = std::max(_rowHeight,   (*view)->getSize().height());
	}

	_size.maxX = _columnWidth*_columns + (_columns - 1)*_spacing;
	_size.maxY = _rowHeight*_rows + (_rows - 1)*_spacing;

	LOG_ALL(gridplacinglog) << "[GridPlacing] column width: " << _columnWidth << std::endl;
	LOG_ALL(gridplacinglog) << "[GridPlacing] row height: " << _rowHeight << std::endl;
	LOG_ALL(gridplacinglog) << "[GridPlacing] final size (with spacing): " << _size << std::endl;

	return _size;
}

template <typename ViewIterator>
void
GridPlacing::draw(const ViewIterator& begin, const ViewIterator& end, const rect<double>& roi, const point<double>& resolution) {

	// the offset for each view
	point<double> offset(0, 0);

	int i = 0;
	for (ViewIterator view = begin; view != end; view++, i++) {

		// the grid position of the view
		int col = i%_columns;
		int row = i/_columns;

		// get the size of the view to draw
		const rect<double>& viewSize = (*view)->getSize();

		LOG_ALL(gridplacinglog) << "[GridPlacing] placing view of size " << viewSize << " at " << row << ", " << col << std::endl;

		// compute the y line offset according to alignment option
		if (_align != TopLeft && _align != TopRight) {

			offset.y = _rowHeight - viewSize.height();

			if (_align == Centered)
				offset.y /= 2.0;

		} else {

			offset.y = 0;
		}

		// compute the x line offset according to alignment option
		if (_align != TopLeft && _align != BottomLeft) {

			offset.x = _columnWidth - viewSize.width();

			if (_align == Centered)
				offset.x /= 2.0;

		} else {

			offset.x = 0;
		}

		LOG_ALL(gridplacinglog) << "[GridPlacing] according to alignment, offset is " << offset << std::endl;

		// add the grid offset
		offset.x += col*(_columnWidth + _spacing);
		offset.y += row*(_rowHeight   + _spacing);

		LOG_ALL(gridplacinglog) << "[GridPlacing] final offset is " << offset << std::endl;

		// draw the view only if it is visible
		if ((viewSize + offset).intersects(roi)) {

			glTranslated(offset.x, offset.y, 0);

			(*view)->draw(roi - offset, resolution);

			glTranslated(-offset.x, -offset.y, 0);
		}
	}
}

} // namespace gui

#endif // GUI_GRID_PLAING_H__

