#include "AsianPayoff.h"
#include <iostream>
#include "PlainVanillaPayoff.h"

AsianPayoff::AsianPayoff(const AsianOption& asianOption, double discountFactor) :
	m_asian_option{asianOption},
	m_strike_observer{asianOption.GetStrikeFixingDates(), asianOption.m_avg_type_strike},
	m_avg_price_observer{asianOption.GetPriceFixingDates(), asianOption.m_avg_type_price},
	m_discount{discountFactor} {}

double AsianPayoff::Evaluate() const {
	// get strike fixing prices
	double strike = m_strike_observer.GetAverage();
	// std::cout <<"Strike: "<< strike << std::endl;

	// get final avg fixing prices
	double avgPrice = m_avg_price_observer.GetAverage();
	// std::cout << "Avg price: " << avgPrice << std::endl;

	// end of the path: evaluate the payoff and discount back
	return m_discount * PlainVanillaPayoff{m_asian_option.m_call_put, strike}(avgPrice);
}

void AsianPayoff::ResetObservers() {
	m_strike_observer.ResetObserver();
	m_avg_price_observer.ResetObserver();
}

std::set<PathObserver*> AsianPayoff::GetPathObservers() {
	return std::set<PathObserver*>{&m_strike_observer, &m_avg_price_observer};
}
