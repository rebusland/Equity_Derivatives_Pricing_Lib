#ifndef _GBM_PATH_GENERATOR_H_
#define _GBM_PATH_GENERATOR_H_

#include "StochasticPathGenerator.h"

#include <cmath>
#include <memory>
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
			std::unique_ptr<UniVariateNumbersGenerator> gaussianGen
		) : StochasticPathGenerator(observationDates),
			m_num_observations{observationDates.size()},
			m_log_spot{std::log(S0)},
			m_gaussian_variate_generator{std::move(gaussianGen)} {
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

		std::unique_ptr<StochasticPathGenerator> clone() const {
			// we use this form (instead of make_unique) in order to exploit the private constructor
			return std::unique_ptr<GBMPathGenerator>(new GBMPathGenerator(*this));
		}

	private:
		// for the sole purpose of cloning
		GBMPathGenerator(const GBMPathGenerator& pathGen) :
			StochasticPathGenerator(pathGen.m_observation_dates),
			m_num_observations{pathGen.m_num_observations},
			m_log_spot{pathGen.m_log_spot},
			m_drifts{pathGen.m_drifts},
			m_vola_widths{pathGen.m_vola_widths}, 
			m_gaussian_variates{pathGen.m_gaussian_variates},
			m_gaussian_variate_generator{pathGen.m_gaussian_variate_generator->clone()} {}

	private:
		const size_t m_num_observations;
		const double m_log_spot;
		std::vector<double> m_drifts;
		std::vector<double> m_vola_widths;
		std::vector<double> m_gaussian_variates;
		std::unique_ptr<UniVariateNumbersGenerator> m_gaussian_variate_generator;
};

#endif
