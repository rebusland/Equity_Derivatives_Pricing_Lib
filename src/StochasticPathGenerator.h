#ifndef _STOCHASTIC_PATH_GENERATOR_H_
#define _STOCHASTIC_PATH_GENERATOR_H_

#include <vector>

using _Date = long;

class StochasticPathGenerator {
	public:
		StochasticPathGenerator(std::vector<_Date> observationDates) :
			m_observation_dates(observationDates) {}

		virtual ~StochasticPathGenerator() {};

		virtual void SimulateRelevantSpotPrices(std::vector<double>& spotPrices) = 0;

	private:
		std::vector<_Date> m_observation_dates;
};

#endif
