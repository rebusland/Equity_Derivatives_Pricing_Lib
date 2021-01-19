#ifndef _PATH_DEPENDENT_PAYOFF_H_
#define _PATH_DEPENDENT_PAYOFF_H_

#include <vector>

using _Date = long;

/**
 * IMPORTANT:
 * The same functional wrapper is used for both path dependent and "State"
 * (i.e. depending only on spot price at expiry) payoffs. Thus, when defining State payoffs
 * we have to artificially use a vector of 1 spot price in contrast to a scalar. This should not affect
 * the performance that much in evaluating the MonteCarlo scenarios, but it simplifies a lot the code.
 */
class Payoff {
	public:
		virtual double operator() (const std::vector<double>& relevantSpotPrices) const = 0;

		/**
		 * IMPORTANT:
		 *  - Observation dates among observers are assumed to be non-overlapping.
		 *  - Containers handling intermediate cash flows are assumed to be chronologically sorted.
		 */
		virtual void FillFlattenedObservationDates() = 0;

		virtual ~Payoff() {}

		// the relevant observation dates to pass to MonteCarlo simulator
		std::vector<_Date> m_flattened_observation_dates;
};

#endif