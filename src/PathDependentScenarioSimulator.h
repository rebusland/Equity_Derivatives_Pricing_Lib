#ifndef _PATH_DEPENDENT_SCENARIO_SIMULATOR_H_
#define _PATH_DEPENDENT_SCENARIO_SIMULATOR_H_

#include <vector>

#include "ScenarioSimulator.h"
#include "PathDependentPayoff.h"

class PathDependentScenarioSimulator : public ScenarioSimulator {
	public:
		PathDependentScenarioSimulator(
			unsigned long nSteps,
			double startPrice,
			const _SdeFunction& sde,
			const VarianceReduction& varianceReduction,
			PathDependentPayoff* payoff
		) : ScenarioSimulator(nSteps, startPrice, sde, varianceReduction),
		m_payoff{payoff}, m_relevant_observation_dates(payoff->m_flattened_observation_dates) {}

		double RunSimulation() override;

	private:
		PathDependentPayoff* m_payoff;
		const std::vector<_Date>& m_relevant_observation_dates;
};

#endif
