#ifndef HORIZONTAL_PLACING_H__
#define HORIZONTAL_PLACING_H__

#include <vector>

#include <util/rect.hpp>
#include <util/point.hpp>

using namespace util;

namespace gui {

/**
 * A placing strategy that places views side-by-side horizontally.
 */
class HorizontalPlacing {

public:

	enum Align {

		Top,
		Centered,
		Bottom
	};

	HorizontalPlacing(double spacing = 0, Align align = Centered) :
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
	double       _spacing;

	// how to align the views vertically
	Align        _align;

	// the offsets for the current views
	std::vector<point<double> > _offsets;
};

/*****************
 * IMPLEMENTAION *
 *****************/

template <typename ViewIterator>
const std::vector<point<double> >&
HorizontalPlacing::getOffsets(const ViewIterator& begin, const ViewIterator& end) {

	_offsets.resize(end - begin);

	if (begin == end)
		return _offsets;

	// get the height of the highest view
	double maxHeight = (*begin)->getSize().height();
	for (ViewIterator view = begin + 1; view != end; view++)
		maxHeight = std::max(maxHeight, (*view)->getSize().height());

	// the offset for each view
	point<double> offset(0, 0);

	unsigned int i = 0;
	for (ViewIterator view = begin; view != end; view++) {

		offset.y = 0;

		// get the size of the view to draw
		const rect<double>& viewSize = (*view)->getSize();

		// compute the y offset
		if (_align != Top) {

			offset.y = maxHeight - viewSize.height();
			
			if (_align == Centered)
				offset.y /= 2.0;
		}

		offset.y -= viewSize.minY;

		_offsets[i].x = offset.x - viewSize.minX;
		_offsets[i].y = offset.y;
		i++;

		offset.x += viewSize.width() + _spacing;
	}

	return _offsets;
}

} // namespace gui

#endif // HORIZONTAL_PLACING_H__

