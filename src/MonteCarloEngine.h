#ifndef _MONTECARLO_ENGINE_H_
#define _MONTECARLO_ENGINE_H_

#include <memory>
#include <vector>

#include "MonteCarloSettings.h"
#include "StatisticsGatherer.h"
#include "StochasticPathGenerator.h"

#include "Payoff.h"

using _Date = long;

class MonteCarloEngine {
	public:
		MonteCarloEngine(
			const size_t nSimul,
			const size_t spotPricesSize,
			std::unique_ptr<StochasticPathGenerator> stochasticPathGenerator,
			std::unique_ptr<Payoff> payoff,
			StatisticsGatherer* statisticsGatherer
		) : m_n_simul{nSimul},
		m_stochastic_path_generator{std::move(stochasticPathGenerator)},
		m_payoff{std::move(payoff)},
		m_statistics_gatherer{statisticsGatherer} {
			// resize spot prices vector
			m_spot_prices.resize(spotPricesSize);
		}

		void operator() () {
			for (size_t i = 0; i < m_n_simul / 2; ++i) {
				// price estimeted in the current scenario
				m_stochastic_path_generator->SimulateRelevantSpotPrices(m_spot_prices);
				const double firstResult{(*m_payoff)(m_spot_prices)};

				m_stochastic_path_generator->SimulateRelevantSpotPrices(m_spot_prices);
				const double secondResult{(*m_payoff)(m_spot_prices)};

				// taking the average of two consecutive simulations lowers variance if using the antithetic variate techniques
				m_statistics_gatherer->AcquireResult((firstResult + secondResult)/2);
			}
		}

	private:
		const size_t m_n_simul;
		std::unique_ptr<StochasticPathGenerator> m_stochastic_path_generator;
		std::unique_ptr<Payoff> m_payoff;
		StatisticsGatherer* m_statistics_gatherer;
		std::vector<double> m_spot_prices;
};

#endif
