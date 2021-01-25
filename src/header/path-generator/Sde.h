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
		Sde(double r, double vola, double dt): m_r{r}, m_vola{vola}, m_dt{dt} {}
		virtual ~Sde() {}

		// The rule for updating the underlying price given its current value 
		// dS = f(S; [dt fixed]); S(t+dt) = S(t) + dS
		virtual double operator() (double S) = 0;

	protected:
		double m_r;
		double m_vola;
		// the update time interval should be "sufficiently small". 
		// A fixed time step is thus assumed in our implementation
		double m_dt;
};

#endif