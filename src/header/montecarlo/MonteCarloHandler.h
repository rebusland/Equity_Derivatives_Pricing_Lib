#ifndef _MONTECARLO_HANDLER_H_
#define _MONTECARLO_HANDLER_H_

#include <memory>
#include <vector>

#include "payoff/Payoff.h"
#include "path-generator/StochasticPathGenerator.h"
#include "statistics-gatherer/PricingResultsGatherer.h"

class MonteCarloHandler {
	public:
		MonteCarloHandler(
			size_t nSimulations,
			unsigned int nThreads,
			std::unique_ptr<StochasticPathGenerator> stochasticPathGenerator,
			std::unique_ptr<Payoff> payoff,
			std::vector<std::unique_ptr<PricingResultsGatherer>>& resultsGatherersPerThread
		);

		// TODO should return the outputs (results + statistics) instead?
		void Run() const;

	private:
		const size_t m_n_simulations;
		const unsigned int m_n_threads;
		std::unique_ptr<StochasticPathGenerator> m_stochastic_path_generator;
		std::unique_ptr<Payoff> m_payoff;
		std::vector<std::unique_ptr<PricingResultsGatherer>>& m_results_gatherers;
};

#endif
