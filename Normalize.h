#ifndef NORMALIZE_H__
#define NORMALIZE_H__

#include <boost/concept_check.hpp>

#include <gui/IsImage.h>

namespace gui {

namespace detail {
/**
 * Image wrapper that divides floating pixel values by a given value.
 */
template <typename Image>
class normalize_impl {

public:

	typedef normalize_impl<Image>      type;
	typedef typename Image::value_type value_type;

	normalize_impl(const Image& ra, const value_type& div) :
		_ra(ra),
		_div(div),
		_height(ra.height()) {}

	const value_type operator()(unsigned int x, unsigned int y) const {

		return _ra(x, y)/_div;
	}

	const unsigned int width() const {

		return _ra.width();
	}

	const unsigned int height() const {

		return _ra.height();
	}

private:

	const Image& _ra;

	value_type   _div;

	unsigned int _height;
};

} // namespace detail

template <typename Image>
typename detail::normalize_impl<Image>::type
normalize(const Image& ra, const typename Image::value_type& value_type) {

	BOOST_CONCEPT_ASSERT((IsImage<Image>));

	return detail::normalize_impl<Image>(ra, value_type);
}

} // namespace gui

#endif // NORMALIZE_H__

