#ifndef GUI_CONVERT_H__
#define GUI_CONVERT_H__

#include <boost/concept_check.hpp>

#include <gui/IsImage.h>

namespace gui {

namespace detail {

/**
 * Image wrapper that divides floating pixel values by a given value.
 */
template <typename target_type, typename Image>
class convert_impl {

public:

	typedef convert_impl<target_type, Image> type;
	typedef target_type                      value_type;

	convert_impl(const Image& ra) :
		_ra(ra) {}

	const target_type operator()(unsigned int x, unsigned int y) const {

		return static_cast<target_type>(_ra(x, y));
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

template <typename target_type, typename Image>
typename detail::convert_impl<target_type, Image>::type
convert(const Image& ra) {

	BOOST_CONCEPT_ASSERT((IsImage<Image>));

	return detail::convert_impl<target_type, Image>(ra);
}

} // namespace gui

#endif // GUI_CONVERT_H__


