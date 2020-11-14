#ifndef _SDE_H_
#define _SDE_H_

/**
 * The stochastic process followed by the underlying during a "sufficiently small" amount of time
 * TODO:
 *  - currently, fixed volatility and interest rate have been assumed. Generalize those to volatility surfaces and underlying interest rate term structure.
 *	- extend development to basket underlying as well
 *
 */
class Sde {
	public:
		Sde(double r, double vola): m_r{r}, m_vola{vola} {}
		virtual ~Sde() {}

		// The rule for updating the underlying price given its current value and the "sufficiently small" time interval
		// dS = f(S, dt), S(t+dt) = S + dS
		virtual double operator() (double S, double dt) = 0;

	protected:
		double m_r;
		double m_vola;
};

#endif