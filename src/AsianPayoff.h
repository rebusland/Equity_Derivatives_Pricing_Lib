#ifndef _ASIAN_PAYOFF_H_
#define _ASIAN_PAYOFF_H_

#include <set>

#include "AsianOption.h"
#include "AsianPathObserver.h"
#include "Payoff.h"
#include "PathObserver.h"

/**
 * TODO:
 * - How to handle fixed strike asian options without creating a different payoff class?
 *
 * NB:
 *  - It is assumed that the validation and consistency-check of all the dates involed 
 * (as well as date sortings) is performed upstream.
 *
 */
class AsianPayoff : public Payoff {
	public:
		AsianPayoff(const AsianOption& asianOption);
		~AsianPayoff() {}

		double Evaluate() const override;

		void ResetObservers() override;

		std::set<PathObserver*> GetPathObservers() override;

	private:
		const AsianOption& m_asian_option;
		AsianPathObserver m_strike_observer;
		AsianPathObserver m_avg_price_observer;
};

#endif
