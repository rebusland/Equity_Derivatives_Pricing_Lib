#ifndef _ASIAN_PAYOFF_H_
#define _ASIAN_PAYOFF_H_

#include "Payoff.h"

#include <memory>

#include "derivative/AsianOption.h"
#include "util/Utils.h"

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
		void FillFlattenedObservationDates() override;
		double operator() (const std::vector<double>& relevantSpotPrices) const override;
		std::unique_ptr<Payoff> Clone() const override;

	private:
		friend class PayoffFactory;

		AsianPayoff(const AsianOption& asianOption, const std::vector<double>& discounts);

		// create a copy: we don't know the lifecycle of the original object
		const AsianOption m_asian_option;

		// preallocate useful variables to evaluate the payoff
		const int m_call_put_flag;
		const size_t m_n_strike_observations;
		const size_t m_n_final_ref_value_observations;
		mutable Utils::RollingAverage m_strike_avg;
		mutable Utils::RollingAverage m_final_ref_price_avg;
};

#endif
