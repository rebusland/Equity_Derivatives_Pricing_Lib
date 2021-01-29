#ifndef _GBM_PATH_GENERATOR_H_
#define _GBM_PATH_GENERATOR_H_

#include "StochasticPathGenerator.h"

#include <cmath>

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
			double S_relative_shift,
			double r,
			double vola,
			std::unique_ptr<UniVariateNumbersGenerator> gaussianGen
		) : StochasticPathGenerator(
				observationDates,
				S0,
				S_relative_shift,
				std::move(gaussianGen)
			) {
			// precompute as much as possible
			m_drifts.resize(m_num_observations);
			m_vola_widths.resize(m_num_observations);

			m_drifts[0] = (r - 0.5*vola*vola) * observationDates[0];
			m_vola_widths[0] = vola * std::sqrt(observationDates[0]);
			for (unsigned int i = 1; i < m_num_observations; ++i) {
				const unsigned long dt = observationDates[i] - observationDates[i - 1];
				m_drifts[i] = (r - 0.5*vola*vola) * dt;
				m_vola_widths[i] = vola * std::sqrt(dt);
			}
		}

		void SimulateRelevantSpotPrices(
			std::vector<double>& spotPrices,
			std::vector<double>& spotPricesDown,
			std::vector<double>& spotPricesUp
		) override {
			m_variates = m_variates_generator->GenerateSequence();

			double expIncrement = 1.0;

			for (unsigned int i = 0; i < m_num_observations; ++i) {
				expIncrement *= std::exp(m_drifts[i] + m_vola_widths[i] * m_variates[i]);
				spotPrices[i] = m_S0 * expIncrement;
				spotPricesDown[i] = m_S_down * expIncrement;
				spotPricesUp[i] = m_S_up * expIncrement;
			}
		}

		std::unique_ptr<StochasticPathGenerator> Clone() const override {
			// we use this form (instead of make_unique) in order to exploit the private constructor
			return std::unique_ptr<GBMPathGenerator>(new GBMPathGenerator(*this));
		}

	private:
		// for the sole purpose of cloning
		GBMPathGenerator(const GBMPathGenerator& pathGen) :
			StochasticPathGenerator(
				pathGen.m_observation_dates,
				pathGen.m_S0,
				pathGen.GetSpotShift(),
				pathGen.m_variates_generator->Clone()
			),
			m_drifts{pathGen.m_drifts},
			m_vola_widths{pathGen.m_vola_widths} {} 

	private:
		std::vector<double> m_drifts;
		std::vector<double> m_vola_widths;
};

#endif
