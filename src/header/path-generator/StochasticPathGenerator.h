#ifndef _STOCHASTIC_PATH_GENERATOR_H_
#define _STOCHASTIC_PATH_GENERATOR_H_

#include "util/pattern/Clonable.h"

#include <memory>
#include <vector>

#include "variate-generator/UniVariateNumbersGenerator.h"

using _Date = long;

class StochasticPathGenerator : public Clonable<StochasticPathGenerator> {
	public:
		StochasticPathGenerator(
			std::vector<_Date> observationDates,
			std::unique_ptr<UniVariateNumbersGenerator> generator
		) : m_observation_dates(observationDates),
			m_num_observations{observationDates.size()},
			m_variates_generator{std::move(generator)} {
				m_variates.resize(m_num_observations);
			}

		virtual ~StochasticPathGenerator() {};

		void SetVariateGeneratorSeed(double seed) {
			m_variates_generator->SetSeed(seed);
		}

		virtual void SimulateRelevantSpotPrices(std::vector<double>& spotPrices) = 0;

	protected:
		std::vector<_Date> m_observation_dates;
		const size_t m_num_observations;
		std::vector<double> m_variates;
		std::unique_ptr<UniVariateNumbersGenerator> m_variates_generator;

};

#endif
