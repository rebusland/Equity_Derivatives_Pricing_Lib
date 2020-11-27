#ifndef _STATE_PAYOFF_H_
#define _STATE_PAYOFF_H_

#include <functional>
#include "Payoff.h"

/**
 * "State" in analogy to thermodynamics, where state functions indicate properties which depend only
 * on the current state of the variables and not on the path followed to reach that state.
 *
 * TODO change it so that it can accept lambdas. Maybe provide static method to return standard
 * payffs like plain vanillas or knock-ins.
 */
class StatePayoff : public Payoff {
	public:
		virtual double operator() (double S) const = 0;
		virtual ~StatePayoff() {}
		
		/*
		// a possibly interesting and useful idea for the future..
		double operator() (double S) {
			return m_payoff_function(S);
		}
		
		std::function<double (double)> m_payoff_function;
		*/
};

#endif
