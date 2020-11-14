#include "AsianPayoff.h"
#include <iostream>
#include "PlainVanillaPayoff.h"

AsianPayoff::AsianPayoff(const AsianOption& asianOption) :
	m_asian_option{asianOption},
	m_strike_observer{asianOption.m_strike_fixing_dates, asianOption.m_avg_type_strike},
	m_avg_price_observer{asianOption.m_price_fixing_dates, asianOption.m_avg_type_price} {}

double AsianPayoff::Evaluate() const {
	// get strike fixing prices
	double strike = m_strike_observer.GetAverage();
	// std::cout <<"Strike: "<< strike << std::endl;

	// get final avg fixing prices
	double avgPrice = m_avg_price_observer.GetAverage();
	// std::cout << "Avg price: " << avgPrice << std::endl;

	// get discount factor at final payment date
	double discount = 1.0; // TODO important: add discount

	// end of the path: evaluate the payoff and discount back
	return discount * PlainVanillaPayoff{m_asian_option.m_call_put, strike, avgPrice}();
}

void AsianPayoff::ResetObservers() {
	m_strike_observer.ResetObserver();
	m_avg_price_observer.ResetObserver();
}

std::set<PathObserver*> AsianPayoff::GetPathObservers() {
	return std::set<PathObserver*>{&m_strike_observer, &m_avg_price_observer};
}
