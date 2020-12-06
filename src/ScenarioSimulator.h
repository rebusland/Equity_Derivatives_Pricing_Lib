#ifndef _SCENARIO_SIMULATOR_H_
#define _SCENARIO_SIMULATOR_H_

#include <functional>

#include "MonteCarloSettings.h"

using _SdeFunction = std::function<double (double)>;

class ScenarioSimulator {
	public:
		ScenarioSimulator(
			int nSteps,
			double startPrice,
			const _SdeFunction& sde,
			const VarianceReduction& varianceReduction
		) :	m_nsteps{nSteps}, m_starting_price{startPrice}, m_sde{sde} {

			switch(varianceReduction) {
				case VarianceReduction::NONE:
					m_run_simulation = std::bind(&ScenarioSimulator::RunSimulationNormal, this);
					break;

				case VarianceReduction::ANTITHETIC:
					m_run_simulation = std::bind(&ScenarioSimulator::RunSimulationAntithetic, this);
					break;

				case VarianceReduction::CONTROL:
					m_run_simulation = std::bind(&ScenarioSimulator::RunSimulationControl, this);
					break;

				default:
					break; // TODO throw exception
			}	
		}

		// dispatcher
		// TODO it is probably more efficient to return the function to run to the caller,
		// so as to avoid this indirection every time the RunSimulation() method is called
		// Could use the switch statement above to return the appropriate Run.. function
		double RunSimulation() {
			return m_run_simulation();
		}

		virtual ~ScenarioSimulator() {}

	protected:
		// VarianceReduction NONE
		virtual double RunSimulationNormal() = 0;

		// VarianceReduction ANTITHETIC
		virtual double RunSimulationAntithetic() = 0;

		// VarianceReduction CONTROL
		virtual double RunSimulationControl() = 0;

		int m_nsteps;
		double m_starting_price;
		const _SdeFunction& m_sde;

	private:
		// which function to run given the variance reduction technique
		std::function<double ()> m_run_simulation;
};

#endif
