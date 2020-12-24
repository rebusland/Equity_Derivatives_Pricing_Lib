#ifndef _SCENARIO_SIMULATOR_H_
#define _SCENARIO_SIMULATOR_H_

#include <functional>

#include "MonteCarloSettings.h"

using _SdeFunction = std::function<double (double)>;

class ScenarioSimulator {
	public:
		ScenarioSimulator(
			unsigned long nSteps,
			double startPrice,
			const _SdeFunction& sde,
			const VarianceReduction& varianceReduction
		) :	m_nsteps{nSteps}, m_starting_price{startPrice}, m_sde{sde} {}

		virtual double RunSimulation() = 0;

		virtual ~ScenarioSimulator() {}

	protected:
		unsigned long m_nsteps;
		double m_starting_price;
		const _SdeFunction& m_sde;
};

#endif
