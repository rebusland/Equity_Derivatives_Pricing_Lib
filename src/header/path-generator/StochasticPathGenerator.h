#ifndef _STOCHASTIC_PATH_GENERATOR_H_
#define _STOCHASTIC_PATH_GENERATOR_H_

#include "util/pattern/Clonable.h"

#include <memory>
#include <vector>

#include "variate-generator/UniVariateNumbersGenerator.h"

#ifndef _DATE_DEF_
#define _DATE_DEF_
using _Date = long;
#endif

class StochasticPathGenerator : public Clonable<StochasticPathGenerator> {
	public:
		StochasticPathGenerator(
			std::vector<_Date> observationDates,
			double S0,
			double S_relative_shift,
			std::unique_ptr<UniVariateNumbersGenerator> generator
		) : m_observation_dates(observationDates),
			m_num_observations{observationDates.size()},
			m_S0{S0},
			m_spot_shift{S_relative_shift},
			m_S_down{m_S0 * (1 - S_relative_shift)},
			m_S_up{m_S0 * (1 + S_relative_shift)},
			m_variates_generator{std::move(generator)} {
				m_variates.resize(m_num_observations);
			}

		virtual ~StochasticPathGenerator() {};

		/*
		 * It's required to set a brand new seed (and thus sequence of variates) among
		 * different threads, where MonteCarlo simulations run
		 */
		void IncrementVariateGeneratorSeed() {
			double newSeed = m_variates_generator->GetSeed() + 1;
			m_variates_generator->SetSeed(newSeed);
		}

		/**
		 * Three sets of spot vectors (normal, up and down scenarios) to compute delta and gamma
		 * TODO (great hassles indeed!):
		 * - How to handle other greeks (vega, rho) and perhaps enable the computations for
		 * 	the additional scenarios only if the greeks are required?
		 * - What if the greeks are computed in a different way (e.g. adjoints)
		 */
		virtual void SimulateRelevantSpotPrices(
			std::vector<double>& spotPrices,
			std::vector<double>& spotPricesDown,
			std::vector<double>& spotPricesUp
		) = 0;

		/**
		 * Absolute spot shift used in up/down scenarios for estimating delta and gamma
		 */
		double GetAbsoluteSpotShift() const {
			return m_spot_shift * m_S0;
		}

		/**
		 * Relative spot shift
		 */
		double GetSpotShift() const {
			return m_spot_shift;
		}

	protected:
		std::vector<_Date> m_observation_dates;
		const size_t m_num_observations;
		const double m_S0;
		const double m_spot_shift;
		const double m_S_down;
		const double m_S_up;
		std::vector<double> m_variates;
		std::unique_ptr<UniVariateNumbersGenerator> m_variates_generator;

};

#endif
