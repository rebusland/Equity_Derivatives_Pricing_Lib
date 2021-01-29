#ifndef _PAYOFF_H_
#define _PAYOFF_H_

#include <vector>

#include "util/pattern/Clonable.h"

#ifndef _DATE_DEF_
#define _DATE_DEF_
using _Date = long;
#endif

/**
 * IMPORTANT:
 * The same functional wrapper is used for both path dependent and "State"
 * (i.e. depending only on spot price at expiry) payoffs. Thus, when defining State payoffs
 * we have to artificially use a vector of 1 spot price in contrast to a scalar. This should not affect
 * the performance that much in evaluating the MonteCarlo scenarios, but it simplifies a lot the code.
 */
class Payoff : public Clonable<Payoff> {
	public:
		Payoff(const std::vector<double>& discounts) : m_discounts(discounts) {}

		virtual void FillFlattenedObservationDates() = 0;

		virtual double operator() (const std::vector<double>& relevantSpotPrices) const = 0;

		virtual ~Payoff() {}

		/**
		 * The relevant observation dates to pass to MonteCarlo simulator
		 * IMPORTANT:
		 *  - Observation dates among observers are assumed to be non-overlapping.
		 *  - Containers handling intermediate cash flows are assumed to be chronologically sorted.
		 */
		std::vector<_Date> m_flattened_observation_dates;

	protected:
		const std::vector<double> m_discounts;
};

#endif
