#include "MonteCarloEngine.h"

#include <iostream>

#include "PathObserver.h"
#include "Utils.h"

/**
 * TODO
 *  - Add handling of VarianceReduction and SimulationScheduler
 *
 */
MonteCarloEngine::MonteCarloEngine(
			const MonteCarloSettings& mcSettings,
			_SdeFunction sdeFunc,
			const _Date& startDate,
			const _Date& endDate,
			double S0,
			Payoff& payoff
		) : m_montecarlo_settings{mcSettings}, m_sde_function{sdeFunc}, m_start_date{startDate},
		m_end_date{endDate}, m_S0{S0}, m_payoff{payoff} {};

double MonteCarloEngine::EvaluatePayoff() {
	Utils::RollingAverage rollingAvgMonteCarlo{0.0, AvgType::ARITHMETIC};

	const int N_SIMUL = m_montecarlo_settings.m_num_simulations;
	double simulationPrice;
	for (int i = 0; i < N_SIMUL; i++) {
		// price estimeted in the current scenario
		simulationPrice = RunSimulation();
		std::cout << "Simulation " << i << " price: " << simulationPrice << "\n";
		rollingAvgMonteCarlo.AddValue(simulationPrice);
	}

	return rollingAvgMonteCarlo.GetAverage();

}

double MonteCarloEngine::RunSimulation() {
	// IMPORTANT, we now assume a time step of a single day
	// Think about how to generalize step size to different intervals
	const double dt = 1;
	// TODO days difference: implement this for realistic _Date representations, should handle stubs and roundings
	const double T = (m_end_date - m_start_date) / dt;
	const int nSteps = T / dt;

	m_payoff.ResetObservers();
	std::set<PathObserver*> pathObservers(m_payoff.GetPathObservers());

	// move step by step through the path
	int i_step = 0;
	double S = m_S0;

	// If all observers are "satisfied", simulation can stop
	while (i_step < nSteps and !pathObservers.empty()) {
		std::cout << i_step << ": " << S << std::endl;

		auto it_observer = pathObservers.begin();
		// Observers are progressively removed from the container as they get completed
		while(it_observer != pathObservers.end()) {
			PathObserver* observer = *it_observer;
			observer->ObservePriceAtDate(i_step, S);

			// Update iterator to refer to the next element
			// The iterator is updated here as it gets invalidated if the current observer is completed (see next lines)
			std::advance(it_observer, 1);
			
			if (observer -> IsCompleted()) {
				// The pointer to the observer is removed from the set, but the underlying object is not destructed. 
				// Set is an associative container (non-sequential linked-list) so only the iterator of the erased element is invalidated
				// NB std::prev(..) as we have advanced it before; std::prev() does not change the original
				pathObservers.erase(std::prev(it_observer));
				std::cout << "Removed path observer. Remaining observers: " << pathObservers.size() << "\n";
			}
		}		

		// step forward: St -> S(t+dt)
		S = S + m_sde_function(S, dt);

		++i_step;
	}

	if (not pathObservers.empty()) {
		// SOMETHING WENT WRONG, EVERY OBSERVER SHOULD HAVE GET ALL THE INFO IT NEEDED and being removed from the observers' set
		// log and fire exception
		std::cerr << "Error in MonteCarlo simulation: some path observer is still pending \n";
	}

	return m_payoff.Evaluate();
}
