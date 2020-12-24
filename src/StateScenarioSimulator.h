#ifndef _STATE_SCENARIO_SIMULATOR_H_
#define _STATE_SCENARIO_SIMULATOR_H_

#include <functional>

#include "ScenarioSimulator.h"

using StatePayoff = std::function<double (double)>;

class StateScenarioSimulator : public ScenarioSimulator {
	public:
		StateScenarioSimulator(
			unsigned long nSteps,
			double startPrice,
			const _SdeFunction& sde,
			const VarianceReduction& varianceReduction,
			StatePayoff* payoff
		) : ScenarioSimulator(nSteps, startPrice, sde, varianceReduction), m_payoff{payoff} {}

		double RunSimulation() override;

	private:
		StatePayoff* m_payoff;
};

#endif
