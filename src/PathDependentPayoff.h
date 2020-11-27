#ifndef _PATH_DEPENDENT_PAYOFF_H_
#define _PATH_DEPENDENT_PAYOFF_H_

#include "Payoff.h"

#include <set>

class PathObserver;

class PathDependentPayoff : public Payoff {
	public:
		virtual double Evaluate() const = 0;

		// TODO: perhaps these two methods are specific to asian payoffs
		virtual std::set<PathObserver*> GetPathObservers() = 0;
		virtual void ResetObservers() = 0;

		virtual ~PathDependentPayoff() {}
};

#endif