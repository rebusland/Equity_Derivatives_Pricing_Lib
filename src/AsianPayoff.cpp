#include "AsianPayoff.h"

#include <algorithm>
#include <iostream>

#include "ProjectException.h"
#include "Utils.h"

AsianPayoff::AsianPayoff(const AsianOption& asianOption, double discountFactor) :
	m_asian_option{asianOption},
	m_discount{discountFactor} {
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

double AsianPayoff::Evaluate(const std::vector<double>& relevantSpotPrices) const {
	// A moving head along relevantSpotPrices vector as it gets traversed.
	unsigned int headIdx = 0;

	// get strike fixing prices
	const unsigned int nStrikeObservations = m_asian_option.GetStrikeFixingDates().size();
	Utils::RollingAverage strikeAvg{0.0, m_asian_option.m_avg_type_strike};
	for (; headIdx < nStrikeObservations; ++headIdx) {
		strikeAvg.AddValue(relevantSpotPrices[headIdx]);
	}
	const double strike = strikeAvg.GetAverage();
	// std::cout <<"Strike: "<< strike << std::endl;

	// get final avg fixing prices
	const unsigned int nFinalRefValueObservations = m_asian_option.GetPriceFixingDates().size();
	Utils::RollingAverage finalRefPriceAvg{0.0, m_asian_option.m_avg_type_price};

	for (unsigned int offset = headIdx; headIdx < offset + nFinalRefValueObservations; ++headIdx) {
		finalRefPriceAvg.AddValue(relevantSpotPrices[headIdx]);
	}
	const double avgPrice = finalRefPriceAvg.GetAverage();
	// std::cout << "Avg price: " << avgPrice << std::endl;

	if (headIdx not_eq relevantSpotPrices.size()) {
		THROW_PROJECT_EXCEPTION("Unobserved spot price while evaluating payoff.");
	}

	// TODO return cash flow instead
	// end of the path: evaluate the payoff and discount back
	return m_discount * m_final_vanilla_payoff(strike, avgPrice);
}
