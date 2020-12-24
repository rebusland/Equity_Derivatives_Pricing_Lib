#include <iostream>

#include "StateScenarioSimulator.h"

double StateScenarioSimulator::RunSimulation() {
	// move step by step through the path
	unsigned long i_step = 0;
	double S = m_starting_price;

	while (i_step < m_nsteps) {
		// std::cout << i_step << ": " << S << std::endl;

		// step forward: St -> S(t+dt)
		S = S + m_sde(S);

		++i_step;
	}
	// std::cout << i_step << ": " << S << std::endl;

	return (*m_payoff)(S);
}
