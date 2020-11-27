#ifndef _STATE_SCENARIO_SIMULATOR_H_
#define _STATE_SCENARIO_SIMULATOR_H_

#include "ScenarioSimulator.h"
#include "StatePayoff.h"

class StateScenarioSimulator : public ScenarioSimulator {
	public:
		StateScenarioSimulator(
			int nSteps,
			double dt,
			double startPrice,
			const _SdeFunction& sde,
			const VarianceReduction& varianceReduction,
			StatePayoff* payoff
		) : ScenarioSimulator(nSteps, dt, startPrice, sde, varianceReduction), m_payoff{payoff} {}

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
