#include "MomentsEvaluator.h"

#include <algorithm>

MomentsEvaluator::MomentsEvaluator(size_t nMoments) : 
	m_rolling_moments{nMoments, RollingAverage{0.0, AvgType::ARITHMETIC}} {}

void MomentsEvaluator::AcquireResult(double simulationResult) {
	double previous = 1;
	for (auto& rollingMoment : m_rolling_moments) {
		// multiplication should be faster than power operator
		rollingMoment.AddValue(previous *= simulationResult);
	}
}

_StatisticalInfoTable MomentsEvaluator::GetStatisticalInfo() const {
	std::vector<double> moments = GetMomentsSoFar();

	_StatisticalInfoTable momentsInfoTable;
	// in this way, everything should be moved (not copied)
	momentsInfoTable.push_back(std::make_unique<_StatisticalInfo>("Moments", std::move(moments)));
	return momentsInfoTable;
}

std::unique_ptr<StatisticsGatherer> MomentsEvaluator::clone() const {
	return std::make_unique<MomentsEvaluator>(*this);
}

std::vector<double> MomentsEvaluator::GetMomentsSoFar() const {
	std::vector<double> moments(m_rolling_moments.size());

	std::transform(
		m_rolling_moments.begin(), m_rolling_moments.end(),
		moments.begin(),
		[](const RollingAverage& rollAvg){ return rollAvg.GetAverage(); }
	);

	return moments;
}
