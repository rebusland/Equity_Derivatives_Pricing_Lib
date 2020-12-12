#ifndef _MONTECARLO_SETTINGS_H_
#define _MONTECARLO_SETTINGS_H_

// TODO find a better name
enum class SimulationScheduler {
	SEQUENTIAL,
	MULTITHREAD
};

enum class VarianceReduction {
	NONE,
	ANTITHETIC,
	CONTROL // i.e. control variate
};

class MonteCarloSettings {
	public:
		MonteCarloSettings(
			const SimulationScheduler& simulationScheduler,
			const VarianceReduction& varianceReduction,
			int numSimulations) :
			m_simulation_scheduler{simulationScheduler},
			m_variance_reduction{varianceReduction},
			m_num_simulations{numSimulations} {}

		int GetNumSimulations() const {
			return m_num_simulations;
		}

	private:
		SimulationScheduler m_simulation_scheduler;
		VarianceReduction m_variance_reduction;
		int m_num_simulations;

		template<typename> friend class MonteCarloEngine;
};

#endif
