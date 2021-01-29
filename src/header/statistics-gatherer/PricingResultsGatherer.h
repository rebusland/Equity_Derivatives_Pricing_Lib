#ifndef _PRICING_RESULTS_GATHERER_H_
#define _PRICING_RESULTS_GATHERER_H_

#include <vector>

#include "util/pattern/Clonable.h"
#include "statistics-gatherer/MomentsEvaluator.h"

/**
 * TODO 
 * This is awful: it's like waving the white flag. How can I setup a more general approach to
 * gather statistics on pricing results for different statistics (e.g. take all the data, take the max and the min
 * values among all simulations ecc.) and for an arbitrary and possibly configurable set of greeks (namely, what
 * if the vega computation can be enabled or disabled in advance)?
 */
class PricingResultsGatherer : public Clonable<PricingResultsGatherer> {
	public:
		PricingResultsGatherer(unsigned int nMoments);

		void AcquirePricingResults(double price, double delta, double gamma);

		std::unique_ptr<PricingResultsGatherer> Clone() const override {
			return std::make_unique<PricingResultsGatherer>(*this);
		}

		std::vector<std::vector<double>> GetPriceAndGreeksMomentsSoFar() const;

	private:
		MomentsEvaluator m_price_evaluator;
		MomentsEvaluator m_delta_evaluator;
		MomentsEvaluator m_gamma_evaluator;		
};

#endif
