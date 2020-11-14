#include "PlainVanillaPayoff.h"

#include<algorithm>

PlainVanillaPayoff::PlainVanillaPayoff(
		const CallPut& callPut,
		double strike,
		double basePrice
	): m_call_put{callPut}, m_strike{strike}, m_base_price{basePrice} {}

double PlainVanillaPayoff::operator() () const {
	int callPutSign = (m_call_put == CallPut::CALL) ? 1 : -1;
	return std::max(0.0, callPutSign * (m_base_price - m_strike));
}
