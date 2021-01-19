#ifndef _PLAIN_VANILLA_PAYOFF_H_
#define _PLAIN_VANILLA_PAYOFF_H_

#include "Payoff.h"

#include <algorithm>
#include <vector>

#include "CallPut.h"

using _Date = long;

/**
 * European plain vanilla
 */
template<CallPut>
class PlainVanillaPayoff : public Payoff {
	public:
		PlainVanillaPayoff(
			const std::vector<double>& discounts,
			_Date expiryDate,
			double strike
		) : Payoff(discounts),
			m_strike{strike} {
			m_flattened_observation_dates.push_back(expiryDate);
		}

		double operator() (const std::vector<double>& spotPrices) const override;

		void FillFlattenedObservationDates() override {/* Nothing to be done for a "maturity-only" payoff */}

	private:
		const double m_strike;
};

template<>
inline double PlainVanillaPayoff<CallPut::CALL>::operator() (const std::vector<double>& spotPrices) const {
	return m_discounts[0] * std::max(0.0, spotPrices[0] - m_strike);
}

template<>
inline double PlainVanillaPayoff<CallPut::PUT>::operator() (const std::vector<double>& spotPrices) const {
	return m_discounts[0] * std::max(0.0, m_strike - spotPrices[0]);
}

#endif
