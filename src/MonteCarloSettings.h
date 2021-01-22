#ifndef _MONTECARLO_SETTINGS_H_
#define _MONTECARLO_SETTINGS_H_

#include <string>

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

enum class VariateGeneratorEnum {
	STD_GAUSS
};

class MonteCarloSettings {
	public:
		MonteCarloSettings(
			VariateGeneratorEnum generatorType,
			double seed,
			const SimulationScheduler& simulationScheduler,
			const VarianceReduction& varianceReduction,
			unsigned long long numSimulations
		) :	m_variate_generator_type{generatorType},
			m_seed{seed},
			m_simulation_scheduler{simulationScheduler},
			m_variance_reduction{varianceReduction},
			m_num_simulations{numSimulations} {}

		VariateGeneratorEnum m_variate_generator_type;
		const double m_seed;
		const SimulationScheduler m_simulation_scheduler;
		const VarianceReduction m_variance_reduction;
		const size_t m_num_simulations;
};

#endif
