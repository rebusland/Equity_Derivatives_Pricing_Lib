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
			const _Date& issueDate,
			const _Date& expiryDate
		) : m_underlying{underlying}, m_issue_date{issueDate}, m_expiry_date{expiryDate} {}

	virtual ~Derivative() {}

	const Underlying m_underlying;
	const _Date m_issue_date;
	const _Date m_expiry_date;
};

#endif
