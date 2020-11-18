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
	CONTROL
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

	private:
		SimulationScheduler m_simulation_scheduler;
		VarianceReduction m_variance_reduction;
		int m_num_simulations;

		friend class MonteCarloEngine;
};

#endif
