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

// TODO associative containers in C++ are not cache friendly, but other structures (such as vectors of pairs) are (probably) inefficient in lookup
// using _RelevantPath = std::vector<double>;

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
		}

		double EvaluatePayoff() {
			// TODO difference in days: implement this for realistic _Date representations.
			// Stubs and holidays should be handled. Remember: the basic time step considered is one day.
			const long T = m_end_date - m_start_date;
			const long nSteps = 1 + T;

			const int N_SIMUL = m_montecarlo_settings.m_num_simulations;

			std::unique_ptr<ScenarioSimulator> scenarioSimulator = BuildScenarioSimulator(nSteps);

			Utils::RollingAverage rollingAvgMonteCarlo{0.0, AvgType::ARITHMETIC};

			double simulationPrice;
			for (int i = 0; i < N_SIMUL; i++) {
				// price estimeted in the current scenario
				simulationPrice = (*scenarioSimulator).RunSimulation();

				rollingAvgMonteCarlo.AddValue(simulationPrice);

				// TODO: enable the statistics gathering only if Debug mode is active(?)
				m_statistics_gatherer.AcquireResult(simulationPrice);
			}

			return rollingAvgMonteCarlo.GetAverage();
		}

	private:
		// this ficticious template declaration is needed in order to resolve the overload with the method below!
		template <typename T = T_Payoff>
		typename std::enable_if<
			std::is_same<T, PathDependentPayoff>::value, // condition for enabling method
			std::unique_ptr<PathDependentScenarioSimulator> // method return type
		>::type
		BuildScenarioSimulator(int nSteps) {
			return std::make_unique<PathDependentScenarioSimulator>(
				nSteps, m_S0, m_sde_function, m_montecarlo_settings.m_variance_reduction, m_payoff);
		}

		template <typename T = T_Payoff>
		typename std::enable_if<
			std::is_same<T, StatePayoff>::value,
			std::unique_ptr<StateScenarioSimulator>
		>::type
		BuildScenarioSimulator(int nSteps) {
			return std::make_unique<StateScenarioSimulator>(
				nSteps, m_S0, m_sde_function, m_montecarlo_settings.m_variance_reduction, m_payoff);
		}

		/**
		 * Generate the Monte Carlo path and returns the whole path.
		 * This overload is useful when the instrument to be priced requires the whole path (or it is extremely path-dependent).
		 *
		 */
		 /*
		std::vector<double> GeneratePath() const {
			// IMPORTANT, we now assume a time step of a single day
			// Think about how to generalize step size to different intervals
			const double dt = 1;
			// TODO days difference implement this for realistic _Date representations, should handle stubs and roundings
			const double T = (m_end_date - m_start_date) / dt;
			const int nSteps = T / dt;

			std::vector<double> path;

			// move step by step through the path
			int i_step = 0;
			double S = m_S0;

			while (i_step < nSteps) {
				// step forward: St -> S(t+dt)
				S = sdeFunc(S, dt);
				path.push_back(S);
				++i_step;
			}

			return path;
		}
		*/

		/**
		 * This overload returns just those path spots which are relevant for the evaluation of the derivative.
		 * TODO The optimal solution for the return type must be found. The type should not be too memory-consuming, while
		 * also allowing from the payoff evaluator code to efficiently retrieve the price information without poor lookup performance.
		 * E.g. map<_Date, double> is efficient in lookup but memory consuming (could maybe lead to cache miss?)
		 * vector<std::pair<_Date, double>> is stored efficiently in memory but may be worst in lookup...
		 */
		 /*
		_RelevantPath GeneratePath(const std::vector<_Date>& observations) const {
			// IMPORTANT, we now assume a time step of a single day
			// Think about how to generalize step size to different intervals
			const double dt = 1;
			// TODO days difference implement this for realistic _Date representations, should handle stubs and roundings
			const double T = (m_end_date - m_start_date) / dt;
			const int nSteps = T / dt;

			_RelevantPath relevantPathInfo;

			// move step by step through the path
			int i_step = 0;
			double S = m_S0;

			// step index representation on the path of the price fixing dates			
			// IMPORTANT: assumed sorted
			std::vector<int> observationIndices = MAGIA(observations); // TODO implement this
			auto it_observations_idx = observationIndices.begin();			

			while (i_step < nSteps) {
				// step forward: St -> S(t+dt)
				S = sdeFunc(S, dt);

				if (it_observations_idx != observationIndices.end() and *it_observations_idx == i_step) {
					relevantPathInfo ---> POPOLA con S e info sulla data;
					std::advance(it_observations_idx, 1);
				}

				++i_step;
			}

			return relevantPathInfo;		
		}
		*/

	private:
		const MonteCarloSettings& m_montecarlo_settings;
		_SdeFunction m_sde_function;
		_Date m_start_date;
		_Date m_end_date;
		double m_S0;
		T_Payoff* m_payoff;
		StatisticsGatherer& m_statistics_gatherer;
};

#endif
