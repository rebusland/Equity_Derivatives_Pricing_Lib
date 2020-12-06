#include "MonteCarloEngine.h"

#include <memory>

#include "PathDependentPayoff.h"
#include "StatePayoff.h"
#include "PathDependentScenarioSimulator.h"
#include "StateScenarioSimulator.h"
#include "Utils.h"

/**
 * TODO
 *  - Add handling of VarianceReduction and SimulationScheduler
 *
 */
MonteCarloEngine::MonteCarloEngine(
			const MonteCarloSettings& mcSettings,
			_SdeFunction sdeFunc,
			_Date startDate,
			_Date endDate,
			double S0,
			Payoff* payoff,
			StatisticsGatherer& statisticsGatherer
		) : m_montecarlo_settings{mcSettings}, m_sde_function{sdeFunc},
		m_start_date{startDate}, m_end_date{endDate}, m_S0{S0}, m_payoff{payoff},
		m_statistics_gatherer{statisticsGatherer} {}

double MonteCarloEngine::EvaluatePayoff() {
	// TODO difference in days: implement this for realistic _Date representations.
	// Stubs and holidays should be handled. Remember: the basic time step considered is one day.
	const double T = m_end_date - m_start_date;
	const int nSteps = 1 + T;

	const VarianceReduction& varianceReduction = m_montecarlo_settings.m_variance_reduction;

	std::unique_ptr<ScenarioSimulator> scenarioSimulator;
	// setup the proper scenario simulator based on the type of payoff
	// NB: if dynamic_cast to the specified type fails a nullptr is return (evaluated to false)
	if (PathDependentPayoff* payoff = dynamic_cast<PathDependentPayoff*>(m_payoff)) {
		scenarioSimulator = std::make_unique<PathDependentScenarioSimulator>(nSteps, m_S0, m_sde_function, varianceReduction, payoff);

	} else if (StatePayoff* payoff = dynamic_cast<StatePayoff*>(m_payoff)) {
		scenarioSimulator = std::make_unique<StateScenarioSimulator>(nSteps, m_S0, m_sde_function, varianceReduction, payoff);

	} else {
		// TODO throw exception for unsupported payoff type
	}

	const int N_SIMUL = m_montecarlo_settings.m_num_simulations;
	Utils::RollingAverage rollingAvgMonteCarlo{0.0, AvgType::ARITHMETIC};

	double simulationPrice;
	for (int i = 0; i < N_SIMUL; i++) {
		// price estimeted in the current scenario
		simulationPrice = (*scenarioSimulator).RunSimulation();
		// std::cout << "Simulation " << i << " price: " << simulationPrice << "\n";
		rollingAvgMonteCarlo.AddValue(simulationPrice);

		// TODO: enable the statistics gathering only if Debug mode is active(?)
		m_statistics_gatherer.AcquireResult(simulationPrice);
	}

	return rollingAvgMonteCarlo.GetAverage();
}
