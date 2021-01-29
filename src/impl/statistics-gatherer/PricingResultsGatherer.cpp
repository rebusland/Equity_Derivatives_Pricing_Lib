#include "statistics-gatherer/PricingResultsGatherer.h"

PricingResultsGatherer::PricingResultsGatherer(unsigned int nMoments) :
	m_price_evaluator{nMoments},
	m_delta_evaluator{nMoments},
	m_gamma_evaluator{nMoments} {}

void PricingResultsGatherer::AcquirePricingResults(double price, double delta, double gamma) {
	m_price_evaluator.AcquireResult(price);
	m_delta_evaluator.AcquireResult(delta);
	m_gamma_evaluator.AcquireResult(gamma);
}

std::vector<std::vector<double>> PricingResultsGatherer::GetPriceAndGreeksMomentsSoFar() const {
	std::vector<std::vector<double>> momentsPerPricingResult;

	momentsPerPricingResult.push_back(m_price_evaluator.GetMomentsSoFar());
	momentsPerPricingResult.push_back(m_delta_evaluator.GetMomentsSoFar());
	momentsPerPricingResult.push_back(m_gamma_evaluator.GetMomentsSoFar());

	return momentsPerPricingResult;
}
