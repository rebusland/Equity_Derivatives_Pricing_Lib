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
			unsigned long long numSimulations) :
			m_simulation_scheduler{simulationScheduler},
			m_variance_reduction{varianceReduction},
			m_num_simulations{numSimulations} {}

		unsigned long long GetNumSimulations() const {
			return m_num_simulations;
		}

	private:
		SimulationScheduler m_simulation_scheduler;
		VarianceReduction m_variance_reduction;
		unsigned long long m_num_simulations;

		friend class MonteCarloEngine;
};

#endif
