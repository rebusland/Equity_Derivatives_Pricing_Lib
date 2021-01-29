#include "montecarlo/MonteCarloEngine.h"

MonteCarloEngine::MonteCarloEngine(
	const size_t nSimul,
	std::unique_ptr<StochasticPathGenerator> stochasticPathGenerator,
	std::unique_ptr<Payoff> payoff,
	PricingResultsGatherer* resultsGatherer
) : m_n_simul{nSimul},
	m_stochastic_path_generator{std::move(stochasticPathGenerator)},
	m_payoff{std::move(payoff)},
	m_results_gatherer{resultsGatherer},
	m_spot_shift{m_stochastic_path_generator->GetAbsoluteSpotShift()} {
		// resize spot prices vector
		const size_t nObservations = m_payoff->m_flattened_observation_dates.size();
		m_spot_prices.resize(nObservations);
		m_spot_prices_down.resize(nObservations);
		m_spot_prices_up.resize(nObservations);
	}

void MonteCarloEngine::operator() () {
	for (size_t i = 0; i < m_n_simul / 2; ++i) {
		// first round
		m_stochastic_path_generator->SimulateRelevantSpotPrices(
			m_spot_prices,
			m_spot_prices_down,
			m_spot_prices_up
		);
		const double firstFairPrice{(*m_payoff)(m_spot_prices)};
		const double firstPriceDown{(*m_payoff)(m_spot_prices_down)};
		const double firstPriceUp{(*m_payoff)(m_spot_prices_up)};

		// TODO can precompute 2 * m_spot_shift and m_spot_shift * m_spot_shift
		const double firstDelta = (firstPriceUp - firstPriceDown) / (2 * m_spot_shift);
		const double firstGamma = (firstPriceUp - 2 * firstFairPrice + firstPriceDown) / (m_spot_shift * m_spot_shift);

		// second round
		m_stochastic_path_generator->SimulateRelevantSpotPrices(
			m_spot_prices,
			m_spot_prices_down,
			m_spot_prices_up
		);
		const double secondFairPrice{(*m_payoff)(m_spot_prices)};
		const double secondPriceDown{(*m_payoff)(m_spot_prices_down)};
		const double secondPriceUp{(*m_payoff)(m_spot_prices_up)};

		const double secondDelta = (secondPriceUp - secondPriceDown) / (2 * m_spot_shift);
		const double secondGamma = (secondPriceUp - 2 * secondFairPrice + secondPriceDown) / (m_spot_shift * m_spot_shift);

		/*
		 * IMPORTANT:
		 * 	Taking the average of two consecutive simulations lowers variance
		 *  (more than the expected effect due to averaging) when using the antithetic variate technique
		 */
		const double midPriceFair = (firstFairPrice + secondFairPrice) / 2;
		const double midDelta = (firstDelta + secondDelta) / 2;
		const double midGamma = (firstGamma + secondGamma) / 2;

		m_results_gatherer->AcquirePricingResults(midPriceFair, midDelta, midGamma);
	}
}
