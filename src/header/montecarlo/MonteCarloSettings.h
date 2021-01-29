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
		class GreeksSettings {
			public:
				GreeksSettings(double spotRelativeShift) : 
					m_spot_relative_shift{spotRelativeShift} {} 

				double m_spot_relative_shift;
		};

		MonteCarloSettings(
			VariateGeneratorEnum generatorType,
			double seed,
			const SimulationScheduler& simulationScheduler,
			unsigned int nThreads,
			const VarianceReduction& varianceReduction,
			size_t numSimulations,
			const MonteCarloSettings::GreeksSettings& greeksSettings
		) :	m_variate_generator_type{generatorType},
			m_seed{seed},
			m_simulation_scheduler{simulationScheduler},
			m_n_threads{nThreads},
			m_variance_reduction{varianceReduction},
			m_num_simulations{numSimulations},
			m_greeks_settings{greeksSettings} {}

		VariateGeneratorEnum m_variate_generator_type;
		const double m_seed;
		const SimulationScheduler m_simulation_scheduler;
		const unsigned int m_n_threads;
		const VarianceReduction m_variance_reduction;
		const size_t m_num_simulations;
		const GreeksSettings m_greeks_settings;
};

#endif
