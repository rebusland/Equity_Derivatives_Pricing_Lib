#ifndef _MONTECARLO_ENGINE_H_
#define _MONTECARLO_ENGINE_H_

#include <functional>
#include <memory>
#include <type_traits>

#include "MonteCarloSettings.h"
#include "PathDependentPayoff.h"
#include "PathDependentScenarioSimulator.h"
#include "ProjectException.h"
#include "StateScenarioSimulator.h"
#include "StatisticsGatherer.h"
#include "Utils.h"

using _Date = long;
using _SdeFunction = std::function<double (double)>;

/**
 * NB, For simulation runner:
 *  - The approach returning the vector of base prices given the observation is probably more
 *	  efficient from an execution-time point of view, while inefficient as the memory usage (every
 *	  time a vector of double is filled and passed by the method call)
 *  - On the other hand, the approach based on path observers uses less memory (evaluation is performed
 *	  in a rolling fashion) while requiring a greater execution time (each observer is monitoring the 
 *	  the path as it's created). BUT, the memory usage depends also on the size of the objects performing the path
 *	  observation. It's not guaranteed that in a multithreaded implementation, all computation "fits"
 * 	  in cache. Moreover, vector is efficient in storing and retrieving data positionally.
 */
template<class T_Payoff>
class MonteCarloEngine {
	public:
		MonteCarloEngine(
			const MonteCarloSettings& mcSettings,
			_SdeFunction sdeFunc,
			_Date startDate,
			_Date endDate,
			double S0,
			T_Payoff* payoff,
			StatisticsGatherer&  statisticsGatherer
		) : m_montecarlo_settings{mcSettings}, m_sde_function{sdeFunc},
		m_start_date{startDate}, m_end_date{endDate}, m_S0{S0}, m_payoff{payoff},
		m_statistics_gatherer{statisticsGatherer} {
			// check template type T_Payoff is a valid choice
			constexpr bool isStatePayoff = std::is_same<T_Payoff, StatePayoff>::value;
			constexpr bool isPathDependentPayoff = std::is_same<T_Payoff, PathDependentPayoff>::value;
			static_assert(
				isStatePayoff or isPathDependentPayoff,
				"Invalid payoff type. Supported types are StatePayoff and PathDependentPayoff"
			);

			// TODO difference in days: implement this for realistic _Date representations.
			// Stubs and holidays should be handled. Remember: the basic time step considered is one day.
			const unsigned long nSteps = m_end_date - m_start_date;

			// initialize the object generating each scenario
			m_scenario_simulator = BuildScenarioSimulator(nSteps);
		}

		void EvaluatePayoff() {
			const unsigned long long N_SIMUL = m_montecarlo_settings.m_num_simulations;

			// Utils::RollingAverage rollingAvgMonteCarlo{0.0, AvgType::ARITHMETIC};
			ScenarioSimulator& scenarioSimulator = *m_scenario_simulator;

			// double simulationPrice;
			for (unsigned long long i = 0; i < N_SIMUL; ++i) {
				// price estimeted in the current scenario
				m_statistics_gatherer.AcquireResult(scenarioSimulator.RunSimulation());
			}
		}

	private:
		// this ficticious template declaration is needed in order to resolve the overload with the method below!
		template <typename T = T_Payoff>
		typename std::enable_if<
			std::is_same<T, PathDependentPayoff>::value, // condition for enabling method
			std::unique_ptr<PathDependentScenarioSimulator> // method return type
		>::type
		BuildScenarioSimulator(unsigned long nSteps) {
			return std::make_unique<PathDependentScenarioSimulator>(
				nSteps, m_S0, m_sde_function, m_montecarlo_settings.m_variance_reduction, m_payoff);
		}

		template <typename T = T_Payoff>
		typename std::enable_if<
			std::is_same<T, StatePayoff>::value,
			std::unique_ptr<StateScenarioSimulator>
		>::type
		BuildScenarioSimulator(unsigned long nSteps) {
			return std::make_unique<StateScenarioSimulator>(
				nSteps, m_S0, m_sde_function, m_montecarlo_settings.m_variance_reduction, m_payoff);
		}

	private:
		const MonteCarloSettings& m_montecarlo_settings;
		_SdeFunction m_sde_function;
		_Date m_start_date;
		_Date m_end_date;
		double m_S0;
		T_Payoff* m_payoff;
		StatisticsGatherer& m_statistics_gatherer;

		std::unique_ptr<ScenarioSimulator> m_scenario_simulator;
};

#endif
