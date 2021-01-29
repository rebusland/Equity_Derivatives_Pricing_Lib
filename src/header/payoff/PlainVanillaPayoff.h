#ifndef _PLAIN_VANILLA_PAYOFF_H_
#define _PLAIN_VANILLA_PAYOFF_H_

#include "Payoff.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "derivative/CallPut.h"
#include "derivative/VanillaOption.h"

#include "util/Utils.h"

/**
 * European plain vanilla
 */
class PlainVanillaPayoff : public Payoff {
	public:
		void FillFlattenedObservationDates() override {/* Nothing to be done */}

		double operator() (const std::vector<double>& spotPrices) const override {
			return m_discounts[0] * std::max(0.0, m_call_put_flag * (spotPrices[0] - m_strike));
		}

		std::unique_ptr<Payoff> Clone() const override {
			return std::make_unique<PlainVanillaPayoff>(*this);
		}

	private:
		friend class PayoffFactory;

		PlainVanillaPayoff(
			const VanillaOption& vanillaOption,
			const std::vector<double>& discounts
		) : Payoff(discounts),
			m_strike{vanillaOption.m_strike},
			m_call_put_flag {Utils::FromCallPutEnumToInt(vanillaOption.m_call_put)} {
			m_flattened_observation_dates.push_back(vanillaOption.m_expiry_date);
		}

		const double m_strike;
		const int m_call_put_flag;
};

#endif
