#ifndef _GBM_PATH_GENERATOR_H_
#define _GBM_PATH_GENERATOR_H_

#include "StochasticPathGenerator.h"

#include <iostream> // TODO remove
#include <cmath>
#include <vector>

#include "UniVariateNumbersGenerator.h"

/**
 * IMPORTANT:
 *  - The observation dates are assumed to be refered relatively to the value date,
 * 	  e.g. if an observation date equals 2, then it means that observation equals value date + 2 days
 */
class GBMPathGenerator : public StochasticPathGenerator {
	public:
		GBMPathGenerator(
			std::vector<_Date> observationDates, 
			double S0,
			double r,
			double vola,
			UniVariateNumbersGenerator* gaussianGen
		) : StochasticPathGenerator(observationDates),
			m_num_observations{observationDates.size()},
			m_log_spot{std::log(S0)},
			m_gaussian_variate_generator{gaussianGen} {
			// precompute as much as possible
			m_drifts.resize(m_num_observations);
			m_vola_widths.resize(m_num_observations);
			m_gaussian_variates.resize(m_num_observations);

			m_drifts[0] = (r - 0.5*vola*vola) * observationDates[0];
			m_vola_widths[0] = vola * std::sqrt(observationDates[0]);
			for (unsigned int i = 1; i < m_num_observations; ++i) {
				const unsigned long dt = observationDates[i] - observationDates[i - 1];
				m_drifts[i] = (r - 0.5*vola*vola) * dt;
				m_vola_widths[i] = vola * std::sqrt(dt);
			}
		}

		void SimulateRelevantSpotPrices(std::vector<double>& spotPrices) override {
			m_gaussian_variates = m_gaussian_variate_generator->GenerateSequence();

			double currentLogSpot = m_log_spot;
			for (unsigned int i = 0; i < m_num_observations; ++i) {
				currentLogSpot += m_drifts[i] + m_vola_widths[i] * m_gaussian_variates[i];
				spotPrices[i] = std::exp(currentLogSpot);
			}
		}

	private:
		const size_t m_num_observations;
		const double m_log_spot;
		std::vector<double> m_drifts;
		std::vector<double> m_vola_widths;
		std::vector<double> m_gaussian_variates;
		UniVariateNumbersGenerator* m_gaussian_variate_generator;
};

#endif
