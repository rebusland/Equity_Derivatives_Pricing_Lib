#ifndef _MONTECARLO_ENGINE_H_
#define _MONTECARLO_ENGINE_H_

#include <functional>

#include "MonteCarloSettings.h"
#include "Payoff.h"
#include "StatisticsGatherer.h"

using _Date = int;

using _SdeFunction = std::function<double (double, double)>;

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

class MonteCarloEngine {
	public:
		MonteCarloEngine(
			const MonteCarloSettings& mcSettings,
			_SdeFunction sdeFunc,
			const _Date& startDate,
			const _Date& endDate,
			double S0,
			Payoff* payoff,
			StatisticsGatherer&  statisticsGatherer
		);

		double EvaluatePayoff();

	private:
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
		

		/**
		 * An interesting and (possibly) clever implementation to decouple payoff logic from path generation,
		 * which does not require to return the whole path or a map containing relevant path spots.
		 * Basically a series of path observers are injected which gather all the information they need as the MC path 
		 * is being constructed along the way. The observers could then update a quantity of interest in a rolling fashion,
		 * thus without requiring too much memory allocation. Concretely, an example of observer could be an asian fixing
		 * observer, which update a rolling fixing value (e.g. an average or min of stock prices) on its observation dates.
		 * Once an observer has gathered all the information he needs, he declares himself as "completed".
		 * Technical note: pathObservers is a list so that we can remove observers from it as they get "satisfied"
		 * while still preserving iterations.
		 *
		 */
		// double RunSimulation();

	private:
		const MonteCarloSettings& m_montecarlo_settings;
		_SdeFunction m_sde_function;
		_Date m_start_date;
		_Date m_end_date;
		double m_S0;
		Payoff* m_payoff;
		StatisticsGatherer& m_statistics_gatherer;
};

#endif
