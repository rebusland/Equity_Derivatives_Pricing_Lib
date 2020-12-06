#ifndef _PATH_DEPENDENT_SCENARIO_SIMULATOR_H_
#define _PATH_DEPENDENT_SCENARIO_SIMULATOR_H_

#include "ScenarioSimulator.h"
#include "PathDependentPayoff.h"

class PathDependentScenarioSimulator : public ScenarioSimulator {
	public:
		PathDependentScenarioSimulator(
			int nSteps,
			double startPrice,
			const _SdeFunction& sde,
			const VarianceReduction& varianceReduction,
			PathDependentPayoff* payoff
		) : ScenarioSimulator(nSteps, startPrice, sde, varianceReduction), m_payoff{payoff} {}

		// VarianceReduction NONE
		double RunSimulationNormal() override;
		// VarianceReduction ANTITHETIC
		double RunSimulationAntithetic() override;
		// VarianceReduction CONTROL
		double RunSimulationControl() override;

	private:
		PathDependentPayoff* m_payoff;
};

#endif
