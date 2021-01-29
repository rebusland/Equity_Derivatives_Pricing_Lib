#ifndef _PRICING_RESULTS_H_
#define _PRICING_RESULTS_H_

#include <string>
#include <valarray>

#include "util/Utils.h"

class PricingResults {
	public:
		class SinglePriceGreekResult {
			public:
				SinglePriceGreekResult(
					const std::valarray<double>& moments,
					size_t nSimulations
				) : m_moments{moments},
					m_mid{moments[0]},
					m_montecarlo_error{
						Utils::GetStdDevOfMeanFromMoments(moments[0], moments[1], nSimulations)
					} {}

				std::valarray<double> m_moments;
				double m_mid;
				double m_montecarlo_error;
		};

		PricingResults(
			const std::valarray<double>& priceMoments,
			const std::valarray<double>& deltaMoments,
			const std::valarray<double>& gammaMoments,
			size_t nSimulations
		) : m_price_result(priceMoments, nSimulations),
			m_delta_result(deltaMoments, nSimulations),
			m_gamma_result(gammaMoments, nSimulations) {}

		SinglePriceGreekResult m_price_result;
		SinglePriceGreekResult m_delta_result;
		SinglePriceGreekResult m_gamma_result;
};

#endif
