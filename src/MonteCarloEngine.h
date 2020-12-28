#ifndef _MONTECARLO_ENGINE_H_
#define _MONTECARLO_ENGINE_H_

#include <functional>
#include <vector>

#include "MonteCarloSettings.h"
#include "PathDependentPayoff.h"
#include "StatisticsGatherer.h"
#include "StochasticPathGenerator.h"

using _Date = long;
using StatePayoff = std::function<double (double)>;

template<class T_Payoff>
class MonteCarloEngine {
	public:
		MonteCarloEngine(
			const MonteCarloSettings& mcSettings,
			StochasticPathGenerator* stochasticPathGenerator,
			T_Payoff* payoff,
			StatisticsGatherer& statisticsGatherer
		) : m_montecarlo_settings{mcSettings},
		m_stochastic_path_generator{stochasticPathGenerator},
		m_payoff{payoff},
		m_statistics_gatherer{statisticsGatherer} {
			// check template type T_Payoff is a valid choice
			constexpr bool isStatePayoff = std::is_same<T_Payoff, StatePayoff>::value;
			constexpr bool isPathDependentPayoff = std::is_same<T_Payoff, PathDependentPayoff>::value;
			static_assert(
				isStatePayoff or isPathDependentPayoff,
				"Invalid payoff type. Supported types are StatePayoff and PathDependentPayoff"
			);

			ResizeSpotPricesVector();
		}

		void EvaluatePayoff() {
			const unsigned long long N_SIMUL = m_montecarlo_settings.m_num_simulations;

			for (unsigned long long i = 0; i < N_SIMUL / 2; ++i) {
				// price estimeted in the current scenario
				m_stochastic_path_generator->SimulateRelevantSpotPrices(m_spot_prices);
				const double firstResult{GetCurrentPayoffValue()};

				m_stochastic_path_generator->SimulateRelevantSpotPrices(m_spot_prices);
				const double secondResult{GetCurrentPayoffValue()};

				// taking the average of two consecutive simulations lowers variance if using the antithetic variate techniques
				m_statistics_gatherer.AcquireResult((firstResult + secondResult)/2);
			}
		}

	private:
		void ResizeSpotPricesVector();
		double GetCurrentPayoffValue() const;

	private:
		const MonteCarloSettings& m_montecarlo_settings;
		StochasticPathGenerator* m_stochastic_path_generator;
		T_Payoff* m_payoff;
		StatisticsGatherer& m_statistics_gatherer;
		std::vector<double> m_spot_prices;
};

/**
 * Template specializations
 */
template<>
inline void MonteCarloEngine<StatePayoff>::ResizeSpotPricesVector() {
	m_spot_prices.resize(1);
}

template<>
inline void MonteCarloEngine<PathDependentPayoff>::ResizeSpotPricesVector() {
	m_spot_prices.resize(m_payoff->m_flattened_observation_dates.size());
}

template<>
inline double MonteCarloEngine<StatePayoff>::GetCurrentPayoffValue() const {
	return (*m_payoff)(m_spot_prices[0]);
}

template<>
inline double MonteCarloEngine<PathDependentPayoff>::GetCurrentPayoffValue() const {
	return m_payoff->Evaluate(m_spot_prices);
}

#endif
