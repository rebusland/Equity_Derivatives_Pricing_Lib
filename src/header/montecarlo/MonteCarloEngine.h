#ifndef _MONTECARLO_ENGINE_H_
#define _MONTECARLO_ENGINE_H_

#include <memory>
#include <vector>

#include "MonteCarloSettings.h"
#include "statistics-gatherer/PricingResultsGatherer.h"
#include "path-generator/StochasticPathGenerator.h"

#include "payoff/Payoff.h"

class MonteCarloEngine {
	public:
		MonteCarloEngine(
			const size_t nSimul,
			std::unique_ptr<StochasticPathGenerator> stochasticPathGenerator,
			std::unique_ptr<Payoff> payoff,
			PricingResultsGatherer* resultsGatherer
		);

		void operator() ();

	private:
		const size_t m_n_simul;
		std::unique_ptr<StochasticPathGenerator> m_stochastic_path_generator;
		std::unique_ptr<Payoff> m_payoff;
		PricingResultsGatherer* m_results_gatherer;

		// the absolute spot shift used to estimate delta and gamma
		const double m_spot_shift;
		std::vector<double> m_spot_prices;
		std::vector<double> m_spot_prices_down;
		std::vector<double> m_spot_prices_up;
};

#endif
