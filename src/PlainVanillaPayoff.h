#ifndef _PLAIN_VANILLA_PAYOFF_H_
#define _PLAIN_VANILLA_PAYOFF_H_

#include "CallPut.h"
#include "StatePayoff.h"

class PlainVanillaPayoff : public StatePayoff {
	public:
		PlainVanillaPayoff(const CallPut& callPut, double strike);
		double operator() (double S) const override;

	private:		
		const CallPut& m_call_put;
		double m_strike;
};

#endif
