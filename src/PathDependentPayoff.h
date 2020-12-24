#ifndef _PATH_DEPENDENT_PAYOFF_H_
#define _PATH_DEPENDENT_PAYOFF_H_

#include <vector>

using _Date = long;

// fwd declaration
class PathDependentScenarioSimulator;

class PathDependentPayoff {
	public:
		virtual double Evaluate(const std::vector<double>& relevantSpotPrices) const = 0;

		/**
		 * IMPORTANT:
		 *  - Observation dates among observers are assumed to be non-overlapping.
		 *  - Containers handling intermediate cash flows are assumed to be chronologically sorted.
		 */
		virtual void FillFlattenedObservationDates() = 0;

		virtual ~PathDependentPayoff() {}

	protected:
		friend class PathDependentScenarioSimulator;

		// the relevant observation dates to pass to MonteCarlo simulator
		std::vector<_Date> m_flattened_observation_dates;
};

#endif