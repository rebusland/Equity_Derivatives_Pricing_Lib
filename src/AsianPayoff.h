#ifndef _ASIAN_PAYOFF_H_
#define _ASIAN_PAYOFF_H_

#include <functional>

#include "AsianOption.h"
#include "Payoff.h"
#include "Utils.h"

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
class AsianPayoff : public Payoff {
	public:
		AsianPayoff(const AsianOption& asianOption, double discountFactor);
		~AsianPayoff() {}

		double operator() (const std::vector<double>& relevantSpotPrices) const override;
		void FillFlattenedObservationDates() override;

	private:
		const AsianOption m_asian_option;
		const double m_discount;

		std::function<double (double, double)> m_final_vanilla_payoff;

		// preallocate useful variables to evaluate the payoff
		const size_t m_n_strike_observations;
		const size_t m_n_final_ref_value_observations;
		mutable Utils::RollingAverage m_strike_avg;
		mutable Utils::RollingAverage m_final_ref_price_avg;
};

#endif
