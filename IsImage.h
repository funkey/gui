#ifndef IS_IMAGE_H__
#define IS_IMAGE_H__

#include <boost/concept_check.hpp>

using namespace boost;

/**
 * Checking class for concept Image.
 */
template <typename Impl>
struct IsImage {

public:

	// has to provide the pixel type
	typedef typename Impl::value_type     value_type;

	// has to provide iterator access
	typedef typename Impl::iterator       iterator;
	typedef typename Impl::const_iterator const_iterator;

	BOOST_CONCEPT_USAGE(IsImage) {

		a = i.begin();
		a = i.end();

		// has to provide its width and height
		unsigned int width  = i.width();
		unsigned int height = i.height();
	}

private:

	Impl i;
	iterator a;
};

#endif // IS_IMAGE_H__

