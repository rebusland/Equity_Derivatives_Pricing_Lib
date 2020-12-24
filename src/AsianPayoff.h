#ifndef _ASIAN_PAYOFF_H_
#define _ASIAN_PAYOFF_H_

#include <functional>

#include "AsianOption.h"
#include "PathDependentPayoff.h"

using _Date = long;

/**
 * TODO:
 * - How to handle fixed strike asian options without creating a different payoff class?
 *
 * NB:
 *  - It is assumed that the validation and consistency-check of all the dates involed 
 * (as well as date sortings) is performed upstream.
 *
 */
class AsianPayoff : public PathDependentPayoff {
	public:
		AsianPayoff(const AsianOption& asianOption, double discountFactor);
		~AsianPayoff() {}

		double Evaluate(const std::vector<double>& relevantSpotPrices) const override;
		void FillFlattenedObservationDates() override;

	private:
		const AsianOption& m_asian_option;
		const double m_discount;

		std::function<double (double, double)> m_final_vanilla_payoff;
};

#endif
