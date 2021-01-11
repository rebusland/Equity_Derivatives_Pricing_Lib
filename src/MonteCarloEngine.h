#ifndef _MONTECARLO_ENGINE_H_
#define _MONTECARLO_ENGINE_H_

#include <functional>
#include <memory>
#include <vector>

#include "MonteCarloSettings.h"
#include "StatisticsGatherer.h"
#include "StochasticPathGenerator.h"

using _Date = long;
using _StatePayoffFunc = std::function<double (double)>;
using _PathDependentPayoffFunc = std::function<double (const std::vector<double>&)>;

template<class T_Payoff>
class MonteCarloEngine {
	public:
		MonteCarloEngine(
			const unsigned long long nSimul,
			size_t spotPricesSize,
			std::unique_ptr<StochasticPathGenerator> stochasticPathGenerator,
			T_Payoff payoff,
			StatisticsGatherer* statisticsGatherer
		) : m_n_simul{nSimul},
		m_stochastic_path_generator{std::move(stochasticPathGenerator)},
		m_payoff{payoff},
		m_statistics_gatherer{statisticsGatherer} {
			// check template type T_Payoff is a valid choice
			constexpr bool isStatePayoff = std::is_same<T_Payoff, _StatePayoffFunc>::value;
			constexpr bool isPathDependentPayoff = std::is_same<T_Payoff, _PathDependentPayoffFunc>::value;
			static_assert(
				isStatePayoff or isPathDependentPayoff,
				"Invalid payoff type. Supported types are _StatePayoff and _PathDependentPayoff"
			);

			// resize spot prices vector
			m_spot_prices.resize(spotPricesSize);
		}

		void operator() () {
			for (unsigned long long i = 0; i < m_n_simul / 2; ++i) {
				// price estimeted in the current scenario
				m_stochastic_path_generator->SimulateRelevantSpotPrices(m_spot_prices);
				const double firstResult{GetCurrentPayoffValue()};

				m_stochastic_path_generator->SimulateRelevantSpotPrices(m_spot_prices);
				const double secondResult{GetCurrentPayoffValue()};

				// taking the average of two consecutive simulations lowers variance if using the antithetic variate techniques
				m_statistics_gatherer->AcquireResult((firstResult + secondResult)/2);
			}
		}

	private:
		double GetCurrentPayoffValue() const;

	private:
		const unsigned long long m_n_simul;
		std::unique_ptr<StochasticPathGenerator> m_stochastic_path_generator;
		T_Payoff m_payoff;
		StatisticsGatherer* m_statistics_gatherer;
		std::vector<double> m_spot_prices;
};

/**
 * Template specializations
 */
template<>
inline double MonteCarloEngine<_StatePayoffFunc>::GetCurrentPayoffValue() const {
	return m_payoff(m_spot_prices[0]);
}

template<>
inline double MonteCarloEngine<_PathDependentPayoffFunc>::GetCurrentPayoffValue() const {
	return m_payoff(m_spot_prices);
}

#endif
