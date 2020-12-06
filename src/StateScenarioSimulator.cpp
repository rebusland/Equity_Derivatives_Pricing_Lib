#include <iostream>

#include "StateScenarioSimulator.h"

double StateScenarioSimulator::RunSimulationNormal() {
	// move step by step through the path
	int i_step = 0;
	double S = m_starting_price;

	while (i_step < m_nsteps) {
		std::cout << i_step << ": " << S << std::endl;

		// step forward: St -> S(t+dt)
		S = S + m_sde(S);

		++i_step;
	}

	return (*m_payoff)(S);
}

double StateScenarioSimulator::RunSimulationAntithetic() {
	// TODO implement
	return -1;
}

double StateScenarioSimulator::RunSimulationControl() {
	// TODO throw exception "Unsopported" instead...and then implement
	return -1;
}
