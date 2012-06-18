#ifndef VERTICAL_PLACING_H__
#define VERTICAL_PLACING_H__

#include <vector>

#include <util/rect.hpp>
#include <util/point.hpp>

using namespace util;

namespace gui {

/**
 * A placing strategy that places views side-by-side vertically.
 */
class VerticalPlacing {

public:

	/**
	 * Used to set the alignment of the content.
	 */
	enum Align {

		Left,
		Right,
		Centered
	};

	/**
	 * Create a new vertical placing strategy.
	 *
	 * @param spacing The space between two views.
	 * @param align <code>Left</code>, <code>Centered</code>, or
	 *              <code>Right</code>: the horizontal alignement of
	 *              the views.
	 */
	VerticalPlacing(double spacing = 0, Align align = Centered) :
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
	 * Get the offsets for each view according to this placing strategy.
	 *
	 * @param begin Begin iterator to collection of views.
	 * @param end End ViewIterator to collection of views.
	 * @return A list of offsets for the placing of the views.
	 */
	template <typename ViewIterator>
	const std::vector<point<double> >& getOffsets(const ViewIterator& begin, const ViewIterator& end);

private:

	// the space between two views
	double _spacing;

	// how to align the views horizontally
	Align  _align;

	// the offsets for the current views
	std::vector<point<double> > _offsets;
};

/*****************
 * IMPLEMENTAION *
 *****************/

template <typename ViewIterator>
const std::vector<point<double> >&
VerticalPlacing::getOffsets(const ViewIterator& begin, const ViewIterator& end) {

	_offsets.resize(end - begin);

	if (begin == end)
		return _offsets;

	// get the width of the widest view
	double maxWidth = (*begin)->getSize().width();
	for (ViewIterator view = begin + 1; view != end; view++)
		maxWidth = std::max(maxWidth, (*view)->getSize().width());

	// the offset for each view
	point<double> offset(0, 0);

	unsigned int i = 0;
	for (ViewIterator view = begin; view != end; view++) {

		offset.x = 0;

		// get the size of the view to draw
		const rect<double>& viewSize = (*view)->getSize();

		// compute the x offset
		if (_align != Left) {

			offset.x = maxWidth - viewSize.width();
			
			if (_align == Centered)
				offset.x /= 2.0;
		}

		offset.x -= viewSize.minX;

		_offsets[i].x = offset.x;
		_offsets[i].y = offset.y - viewSize.minY;
		i++;

		offset.y += viewSize.height() + _spacing;
	}

	return _offsets;
}

} // namespace gui

#endif // VERTICAL_PLACING_H__

