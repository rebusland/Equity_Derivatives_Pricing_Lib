#include "AsianPayoff.h"
#include <algorithm>
#include <iostream>

AsianPayoff::AsianPayoff(const AsianOption& asianOption, double discountFactor) :
	m_asian_option{asianOption},
	m_strike_observer{asianOption.GetStrikeFixingDates(), asianOption.m_avg_type_strike},
	m_avg_price_observer{asianOption.GetPriceFixingDates(), asianOption.m_avg_type_price},
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
	}

double AsianPayoff::Evaluate() const {
	// get strike fixing prices
	const double strike = m_strike_observer.GetAverage();
	// std::cout <<"Strike: "<< strike << std::endl;

	// get final avg fixing prices
	const double avgPrice = m_avg_price_observer.GetAverage();
	// std::cout << "Avg price: " << avgPrice << std::endl;

	// end of the path: evaluate the payoff and discount back
	return m_discount * m_final_vanilla_payoff(strike, avgPrice);
}

void AsianPayoff::ResetObservers() {
	m_strike_observer.ResetObserver();
	m_avg_price_observer.ResetObserver();
}

std::set<PathObserver*> AsianPayoff::GetPathObservers() {
	return std::set<PathObserver*>{&m_strike_observer, &m_avg_price_observer};
}
