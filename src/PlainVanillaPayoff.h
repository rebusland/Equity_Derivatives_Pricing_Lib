#ifndef _PLAIN_VANILLA_PAYOFF_H_
#define _PLAIN_VANILLA_PAYOFF_H_

#include<algorithm>

#include "CallPut.h"

class PlainVanillaPayoff {
	public:
		PlainVanillaPayoff(
			const CallPut& callPut,
			double strike
		) : m_call_put_sign{callPut == CallPut::CALL ? 1 : -1}, m_strike{strike} {}

		double operator() (double S) const {
			return std::max(0.0, m_call_put_sign * (S - m_strike));
		}

	private:
		const int m_call_put_sign;
		const double m_strike;
};

#endif
