#include "MomentsEvaluator.h"

#include <algorithm>

#include "ProjectException.h"

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
	momentsInfoTable.push_back(_StatisticalInfo(MOMENTS_STRING, std::move(moments)));
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

_StatisticalInfo MomentsEvaluator::MergeMomentsInfo(const std::vector<std::vector<double>>& momentsInfoVec) {
	const size_t nElementsToMerge = momentsInfoVec.size();

	if (nElementsToMerge == 0) {
		THROW_PROJECT_EXCEPTION("No valid moments to merge");
	} 

	// TODO validate that all the vectors of moments to merge have the same size
	const size_t nMoments = momentsInfoVec[0].size();

	std::vector<double> mergedMoments(nMoments, 0);
	for (const auto& moments : momentsInfoVec) {
		for (unsigned int i = 0; i < nMoments; ++i) {
			mergedMoments[i] += moments[i];
		}
	}

	std::for_each(
		mergedMoments.begin(), mergedMoments.end(),
		[=](auto& moment) {moment /= nElementsToMerge;}
	);

	return _StatisticalInfo(MOMENTS_STRING, std::move(mergedMoments));
}
