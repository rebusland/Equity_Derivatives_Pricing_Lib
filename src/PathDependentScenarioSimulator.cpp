#include <iostream>

#include "PathObserver.h"
#include "PathDependentScenarioSimulator.h"

/**
 * A (possibly) clever implementation to decouple payoff logic from path generation,
 * which does not require to return the whole path or a map containing relevant path spots.
 * Basically, a series of path observers are injected which gather all the information they need as the MC path 
 * is being constructed along the way. The observers could then update a quantity of interest in a rolling fashion,
 * thus without requiring too much memory allocation. Concretely, an example of observer could be an asian fixing
 * observer, which update a rolling fixing value (e.g. an average or min of stock prices) on its observation dates.
 * Once an observer has gathered all the information he needs, he declares himself as "completed".
 * Technical note: pathObservers is a set so that we can remove observers from it as they get "satisfied"
 * while still preserving iterations.
 *
 */
double PathDependentScenarioSimulator::RunSimulationNormal() {
	m_payoff->ResetObservers();
	std::set<PathObserver*> pathObservers(m_payoff->GetPathObservers());

	// move step by step through the path
	int i_step = 0;
	double S = m_starting_price;

	// If all observers are "satisfied", simulation can stop
	// NB <= condition: needed by the observers to get the price at the final date
	while (i_step <= m_nsteps and not pathObservers.empty()) {
		// std::cout << i_step << ": " << S << std::endl;

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
				// std::cout << "Removed path observer. Remaining observers: " << pathObservers.size() << "\n";
			}
		}

		// observers watched the whole path: we can quit the loop
		if (i_step == m_nsteps)
			break;

		// step forward: St -> S(t+dt)
		S = S + m_sde(S);

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


