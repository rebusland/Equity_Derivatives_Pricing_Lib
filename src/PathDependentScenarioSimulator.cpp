#include <algorithm>
#include <iostream>

#include "PathDependentScenarioSimulator.h"

double PathDependentScenarioSimulator::RunSimulation() {
	std::vector<double> relevantSpotPrices;

	// move step by step through the path
	unsigned long i_step = 0;
	double S = m_starting_price;

	while (i_step < m_nsteps) {
		// std::cout << i_step << ": " << S << std::endl;

		/*
		 * If the vector is ordered, as in this case, we can exploit binary search algo, which scales as log(n)
		 */
		if (std::binary_search(
				m_relevant_observation_dates.begin(),
				m_relevant_observation_dates.end(),
				i_step)
		) {
			relevantSpotPrices.push_back(S);
		}

		// step forward: St -> S(t+dt)
		S = S + m_sde(S);

		++i_step;
	}

	// check repeated to observe, if needed, the final spot price
	if (std::binary_search(
			m_relevant_observation_dates.begin(),
			m_relevant_observation_dates.end(),
			i_step)
	) {
		relevantSpotPrices.push_back(S);
	}

	return m_payoff->Evaluate(relevantSpotPrices);
}
