#ifndef _DERIVATIVE_H_
#define _DERIVATIVE_H_

#include "Underlying.h"

using _Date = int;

/**
 * TODO:
 *  - Add handling of Quanto instruments
 *  - Add handling of composite underlyings (baskets): this requires correlation etc.
 */
class Derivative {
	public:
		Derivative(
			const Underlying& underlying,
			const _Date& startDate,
			const _Date& expiryDate
		) : m_underlying{underlying}, m_start_date{startDate}, m_expiry_date{expiryDate} {}

	virtual ~Derivative() {}

	// private:
		Underlying m_underlying;
		_Date m_start_date;
		_Date m_expiry_date;
};

#endif
