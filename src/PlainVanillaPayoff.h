#ifndef _PLAIN_VANILLA_PAYOFF_H_
#define _PLAIN_VANILLA_PAYOFF_H_

#include "CallPut.h"

class PlainVanillaPayoff {
	public:
		PlainVanillaPayoff(const CallPut& callPut, double strike, double basePrice);
		double operator() () const;

	private:		
		const CallPut& m_call_put;
		double m_strike;
		double m_base_price;
};

#endif
