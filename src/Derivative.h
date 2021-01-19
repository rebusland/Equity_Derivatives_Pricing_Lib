#ifndef _DERIVATIVE_H_
#define _DERIVATIVE_H_

#include <string>

#include "Underlying.h"

using _Date = long;

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
			const _Date& expiryDate,
			std::string payoffId
		) : m_underlying{underlying},
			m_issue_date{issueDate},
			m_expiry_date{expiryDate},
			m_payoff_id{payoffId} {}

	virtual ~Derivative() {}

	const Underlying m_underlying;
	const _Date m_issue_date;
	const _Date m_expiry_date;
	const std::string m_payoff_id;
};

#endif
