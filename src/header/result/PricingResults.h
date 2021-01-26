#ifndef _PRICING_RESULTS_H_
#define _PRICING_RESULTS_H_

#include <string>
#include <vector>

#include "io/JSONWriter.h"

class PricingResults {
	public:
		PricingResults(double fairPrice, double mcError, std::vector<double> moments):
			m_fair_price{fairPrice}, m_montecarlo_error{mcError}, m_moments{moments} {}

		double m_fair_price;
		double m_montecarlo_error;
		std::vector<double> m_moments;
};

#endif
