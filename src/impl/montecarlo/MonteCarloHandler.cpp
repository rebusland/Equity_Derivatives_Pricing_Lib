#include "montecarlo/MonteCarloHandler.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <thread>

#include "montecarlo/MonteCarloEngine.h"

// TODO IMPORTANT: we assume that the minimum time step is a single day.
// This is the time step used by default in the underlying SDE's equation, generating the MC paths.
constexpr double MIN_TIME_STEP = 1;

MonteCarloHandler::MonteCarloHandler(
	size_t nSimulations,
	unsigned int nThreads,
	std::unique_ptr<StochasticPathGenerator> pathGenerator,
	std::unique_ptr<Payoff> payoff,
	std::vector<std::unique_ptr<PricingResultsGatherer>>& resultsGatherersPerThreadRef
) : m_n_simulations{nSimulations}, m_n_threads{nThreads},
	m_stochastic_path_generator(std::move(pathGenerator)),
	m_payoff(std::move(payoff)),
	m_results_gatherers(resultsGatherersPerThreadRef) {}

void MonteCarloHandler::Run() const {

	/*
	 * Start multi-threading routine
	 */

	// TODO ignore rounding error (for a high number of simulations this is negligible)
	const size_t nSimulPerThread = m_n_simulations / m_n_threads;

	std::vector<std::thread> thread_vec;
	for (const auto& resultGatherer : m_results_gatherers) {
		std::unique_ptr<StochasticPathGenerator> pathGenerator = m_stochastic_path_generator->Clone();

		// TODO: find a cleaner and more reasoned way to set a different seed in each thread
		pathGenerator->IncrementVariateGeneratorSeed();

		MonteCarloEngine mcEngine{
			nSimulPerThread,
			std::move(pathGenerator),
			m_payoff->Clone(),
			resultGatherer.get()
		};
		thread_vec.emplace_back(std::move(mcEngine));
	}

	// join all threads
	std::for_each(thread_vec.begin(), thread_vec.end(), [](auto& thr){ thr.join(); });
}
