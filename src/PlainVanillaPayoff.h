#ifndef _PLAIN_VANILLA_PAYOFF_H_
#define _PLAIN_VANILLA_PAYOFF_H_

#include<algorithm>

#include "CallPut.h"

template<CallPut>
class PlainVanillaPayoff {
	public:
		PlainVanillaPayoff(double strike) : m_strike{strike} {}
		double operator() (double S) const;

	private:
		const double m_strike;
};

template<>
inline double PlainVanillaPayoff<CallPut::CALL>::operator() (double S) const {
	return std::max(0.0, S - m_strike);
}

template<>
inline double PlainVanillaPayoff<CallPut::PUT>::operator() (double S) const {
	return std::max(0.0, m_strike - S);
}

#endif
