#include "AsianPayoff.h"

#include <algorithm>
#include <iostream>

AsianPayoff::AsianPayoff(const AsianOption& asianOption, double discountFactor) :
	m_asian_option{asianOption},
	m_discount{discountFactor},
	m_n_strike_observations{m_asian_option.GetStrikeFixingDates().size()},
	m_n_final_ref_value_observations{m_asian_option.GetPriceFixingDates().size()},
	m_strike_avg{0.0, m_asian_option.m_avg_type_strike},
	m_final_ref_price_avg{0.0, m_asian_option.m_avg_type_price}
	{
		// It's not elegant but we avoid the inefficiency of creating and deleting at each 
		// simulation a PlainVanillaPayoff object (which is an expensive operation).
		if (m_asian_option.m_call_put == CallPut::CALL) {
			m_final_vanilla_payoff = [](double K, double S) {
				return std::max(0.0, S - K);
			};
		} else {
			m_final_vanilla_payoff = [](double K, double S) {
				return std::max(0.0, K - S);
			};
		}

		FillFlattenedObservationDates();
	}

void AsianPayoff::FillFlattenedObservationDates() {
	// strike observations
	const std::vector<_Date>& strikeObservations = m_asian_option.GetStrikeFixingDates();
	m_flattened_observation_dates.insert(
		m_flattened_observation_dates.end(),
		strikeObservations.begin(),
		strikeObservations.end()
	);

	// final reference price observations
	const std::vector<_Date>& finalRefPriceObservations = m_asian_option.GetPriceFixingDates();
	m_flattened_observation_dates.insert(
		m_flattened_observation_dates.end(),
		finalRefPriceObservations.begin(),
		finalRefPriceObservations.end()
	);
}

double AsianPayoff::operator() (const std::vector<double>& relevantSpotPrices) const {
	// A moving head along relevantSpotPrices vector as it gets traversed.
	unsigned int headIdx = 0;

	// get strike fixing prices
	m_strike_avg.ResetAverage(0.0); // it's cheaper to reset the rolling avgs than to recreate them each time
	for (; headIdx < m_n_strike_observations; ++headIdx) {
		m_strike_avg.AddValue(relevantSpotPrices[headIdx]);
	}
	const double strike = m_strike_avg.GetAverage();
	// std::cout <<"Strike: "<< strike << std::endl;

	// get final avg fixing prices
	m_final_ref_price_avg.ResetAverage(0.0);
	for (unsigned int offset = headIdx; headIdx < offset + m_n_final_ref_value_observations; ++headIdx) {
		m_final_ref_price_avg.AddValue(relevantSpotPrices[headIdx]);
	}
	const double avgPrice = m_final_ref_price_avg.GetAverage();
	// std::cout << "Avg price: " << avgPrice << std::endl;

	// TODO return cash flow instead and apply discount in the caller code (?)
	// end of the path: evaluate the payoff and discount back
	return m_discount * m_final_vanilla_payoff(strike, avgPrice);
}
