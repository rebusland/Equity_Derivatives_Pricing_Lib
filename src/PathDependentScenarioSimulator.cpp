#include <iostream>

#include "PathObserver.h"
#include "PathDependentScenarioSimulator.h"

double PathDependentScenarioSimulator::RunSimulationNormal() {
	m_payoff->ResetObservers();
	std::set<PathObserver*> pathObservers(m_payoff->GetPathObservers());

	// move step by step through the path
	int i_step = 0;
	double S = m_starting_price;

	// If all observers are "satisfied", simulation can stop
	while (i_step < m_nsteps and !pathObservers.empty()) {
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
		S = S + m_sde(S, m_dt);

		++i_step;
	}

	if (not pathObservers.empty()) {
		// SOMETHING WENT WRONG, EVERY OBSERVER SHOULD HAVE GET ALL THE INFO IT NEEDED and being removed from the observers' set
		// log and fire exception
		std::cerr << "Error in MonteCarlo simulation: some path observer is still pending \n";
	}

	return m_payoff->Evaluate();
}

double PathDependentScenarioSimulator::RunSimulationAntithetic() {
	// TODO implement
	return -1;
}

double PathDependentScenarioSimulator::RunSimulationControl() {
	// TODO throw exception "Unsopported" instead...and then implement
	return -1;
}


