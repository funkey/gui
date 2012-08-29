#ifndef OVERLAY_PLACING_H__
#define OVERLAY_PLACING_H__

#include <vector>

#include <util/point.hpp>

using namespace util;

namespace gui {

/**
 * A placing strategy that places views one above another without any rescaling
 * or translation.
 */
class OverlayPlacing {

public:

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

	// the offsets for the current views
	std::vector<point<double> > _offsets;
};

/*****************
 * IMPLEMENTAION *
 *****************/

template <typename ViewIterator>
const std::vector<point<double> >&
OverlayPlacing::getOffsets(const ViewIterator& begin, const ViewIterator& end) {

	// there's not too much to do here -- the offsets are all zero
	_offsets.resize(end - begin, point<double>(0, 0));

	return _offsets;
}

} // namespace gui

#endif // OVERLAY_PLACING_H__
