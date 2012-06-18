#ifndef HORIZONTAL_FLIP_H__
#define HORIZONTAL_FLIP_H__

#include <boost/concept_check.hpp>

#include <gui/IsImage.h>

namespace gui {

namespace detail {

/**
 * Image wrapper to flip the content of an image horizontally.
 */
template <typename Image>
class horizontal_flip_impl {

public:

	typedef horizontal_flip_impl<Image> type;
	typedef typename Image::value_type  value_type;

	horizontal_flip_impl(const Image& ra) :
		_ra(ra) {}

	const value_type& operator()(unsigned int x, unsigned int y) const {

		return _ra(x, _ra.height() - 1 - y);
	}

	const unsigned int width() const {

		return _ra.width();
	}

	const unsigned int height() const {

		return _ra.height();
	}

private:

	const Image& _ra;
};

} // namespace detail

template <typename Image>
typename detail::horizontal_flip_impl<Image>::type
horizontal_flip(const Image& image) {

	BOOST_CONCEPT_ASSERT((IsImage<Image>));

	return detail::horizontal_flip_impl<Image>(image);
}

} // namespace gui

#endif // HORIZONTAL_FLIP_H__

