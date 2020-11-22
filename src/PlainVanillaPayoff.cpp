#include "PlainVanillaPayoff.h"

#include<algorithm>

PlainVanillaPayoff::PlainVanillaPayoff(
		const CallPut& callPut,
		double strike
	): m_call_put{callPut}, m_strike{strike} {}

double PlainVanillaPayoff::operator() (double S) const {
	int callPutSign = (m_call_put == CallPut::CALL) ? 1 : -1;
	return std::max(0.0, callPutSign * (S - m_strike));
}
