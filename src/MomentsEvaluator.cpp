#include "MomentsEvaluator.h"

#include <algorithm>

MomentsEvaluator::MomentsEvaluator(size_t nMoments) : 
	m_rolling_moments{nMoments, RollingAverage{0.0, AvgType::ARITHMETIC}} {}

void MomentsEvaluator::AcquireResult(double simulationResult) {
	double moment = simulationResult;
	for (unsigned i = 0; i < m_rolling_moments.size(); i++) {
		m_rolling_moments[i].AddValue(moment);

		// multiplication should be faster than power operator
		moment *= simulationResult;
	}
}

_StatisticalInfoTable MomentsEvaluator::GetStatisticalInfo() const {
	std::vector<double> moments(4);

	std::transform(
		m_rolling_moments.begin(), m_rolling_moments.end(),
		moments.begin(),
		[](const RollingAverage& rollAvg){ return rollAvg.GetAverage(); }
	);

	_StatisticalInfo momentsInfo{"Moments", moments};
	return _StatisticalInfoTable(1, momentsInfo);
}
