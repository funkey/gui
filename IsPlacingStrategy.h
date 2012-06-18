#ifndef IS_PLACING_STRATEGY_H__
#define IS_PLACING_STRATEGY_H__

/**
 * Checking class for the concept IsPlacingStrategy.
 */

#include <boost/concept_check.hpp>

template <typename Impl>
struct IsPlacingStrategy : boost::DefaultConstructible<Impl> {

public:

	BOOST_CONCEPT_USAGE(IsPlacingStrategy) {

		// should have methods:
		//i.resetPlacing();
		//i.placeAndDraw(...);
		//i.computeSize();
	};

//private:

	//Impl i;

};

#endif // IS_PLACING_STRATEGY_H__

