#include "AsianPayoff.h"

#include <algorithm>
#include <iostream>

#include "Utils.h"

AsianPayoff::AsianPayoff(const AsianOption& asianOption, const std::vector<double>& discounts) :
	Payoff(discounts),
	m_asian_option{asianOption},
	m_call_put_flag{Utils::FromCallPutEnumToInt(m_asian_option.m_call_put)},
	m_n_strike_observations{m_asian_option.GetStrikeFixingDates().size()},
	m_n_final_ref_value_observations{m_asian_option.GetPriceFixingDates().size()},
	m_strike_avg{0.0, m_asian_option.m_avg_type_strike},
	m_final_ref_price_avg{0.0, m_asian_option.m_avg_type_price} {
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

	/* end of the path: evaluate the payoff and discount back
	 * TODO return cash flow instead and apply discount in the caller code (?)
	 */
	// We avoid the cost of creating and deleting at each simulation a PlainVanillaPayoff object 
	return m_discounts[0] * std::max(0.0, m_call_put_flag * (avgPrice - strike));
}

std::unique_ptr<Payoff> AsianPayoff::Clone() const {
	return std::make_unique<AsianPayoff>(*this);
}
