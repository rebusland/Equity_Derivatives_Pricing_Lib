#include "util/Utils.h"

#include <cmath>
#include <fstream>
#include <numeric>
#include <sstream>
#include <valarray>

#include "result/PricingResults.h"
#include "statistics-gatherer/PricingResultsGatherer.h"

double Utils::ComputeAverage(const std::vector<double>& data, const AvgType& avgType) {
	unsigned int n = data.size();

	if (avgType == AvgType::GEOMETRIC) {
		auto logSumLambda = [](double init, double elem){return init + std::log(elem);};
		return std::exp(std::accumulate(data.cbegin(), data.cend(), 0.0, logSumLambda) / n);
	}

	// Simple Arithmetic Average
	return std::accumulate(data.cbegin(), data.cend(), 0.0) / n;
}

Utils::RollingAverage::RollingAverage(double value, const AvgType& avgType):
	m_value{value}, m_avg_type{avgType} {}

void Utils::RollingAverage::AddValue(double elem) {
	// increment counter
	++m_counter;

	if (m_avg_type == AvgType::GEOMETRIC) {
		m_value += std::log(elem);
		return;
	}

	// Simple Arithmetic Average
	m_value += elem;
}

double Utils::RollingAverage::AddValueAndGetAverage(double elem) {
	AddValue(elem);
	return GetAverage();
}

double Utils::RollingAverage::GetAverage() const {
	if (m_avg_type == AvgType::GEOMETRIC) {
		return std::exp(m_value / (double) m_counter);
	}

	// simple arithmetic average
	return m_value / (double) m_counter;
}

void Utils::RollingAverage::ResetAverage(double resetValue) {
	m_value = resetValue;
	m_counter = 0;
}

double Utils::GetStdDevOfMeanFromMoments(double m1, double m2, size_t N) {
	return std::sqrt((m2 - m1 * m1) / (N - 1));
}

PricingResults Utils::ExtractPricingResultsFromGatherers(
	const std::vector<std::unique_ptr<PricingResultsGatherer>>& resultsGatherersPerThread,
	unsigned int nThreads,
	size_t nMoments,
	size_t nMCSimulations
) {
	std::vector<std::valarray<double>> priceMomentsVec;
	std::vector<std::valarray<double>> deltaMomentsVec;
	std::vector<std::valarray<double>> gammaMomentsVec;

	for (auto& gathererUp : resultsGatherersPerThread) {
		const std::vector<std::vector<double>> threadPriceGreeksMoments = gathererUp->GetPriceAndGreeksMomentsSoFar();
		priceMomentsVec.emplace_back(threadPriceGreeksMoments[0].data(), nMoments);
		deltaMomentsVec.emplace_back(threadPriceGreeksMoments[1].data(), nMoments);
		gammaMomentsVec.emplace_back(threadPriceGreeksMoments[2].data(), nMoments);
	}

	// average results among different threads
	// NB nThreads should be == priceMomentsVec.size() == deltaMomentsVec.size() == gammaMomentsVec.size()
	std::valarray<double> priceMoments = priceMomentsVec[0];
	std::valarray<double> deltaMoments = deltaMomentsVec[0];
	std::valarray<double> gammaMoments = gammaMomentsVec[0];
	for (unsigned int i = 1; i < nThreads; ++i) {
		priceMoments += priceMomentsVec[i];
		deltaMoments += deltaMomentsVec[i];
		gammaMoments += gammaMomentsVec[i];
	}
	priceMoments /= nThreads;
	deltaMoments /= nThreads;
	gammaMoments /= nThreads;

	return PricingResults(
		priceMoments,
		deltaMoments,
		gammaMoments,
		nMCSimulations
	);
}

std::string Utils::ReadJsonFileAsString(const char* relativePathFileName) {
	std::ifstream f(relativePathFileName);
	std::string str;
	if(f) {
		std::ostringstream ss;
		ss << f.rdbuf();
		str = ss.str();
	}

	return str;
}
