#ifndef _STATE_SCENARIO_SIMULATOR_H_
#define _STATE_SCENARIO_SIMULATOR_H_

#include <functional>

#include "ScenarioSimulator.h"

using StatePayoff = std::function<double (double)>;

class StateScenarioSimulator : public ScenarioSimulator {
	public:
		StateScenarioSimulator(
			long nSteps,
			double startPrice,
			const _SdeFunction& sde,
			const VarianceReduction& varianceReduction,
			StatePayoff* payoff
		) : ScenarioSimulator(nSteps, startPrice, sde, varianceReduction), m_payoff{payoff} {}

		// VarianceReduction NONE
		double RunSimulationNormal() override;
		// VarianceReduction ANTITHETIC
		double RunSimulationAntithetic() override;
		// VarianceReduction CONTROL
		double RunSimulationControl() override;

	private:
		StatePayoff* m_payoff;
};

#endif
