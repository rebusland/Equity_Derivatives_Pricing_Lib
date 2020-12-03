#include "CompositeStatisticsGatherer.h"

#include <algorithm>

CompositeStatisticsGatherer::CompositeStatisticsGatherer(
		std::initializer_list<StatisticsGatherer*> statsGatherers
	) :	m_stats_gatherers(statsGatherers) {}

void CompositeStatisticsGatherer::AcquireResult(double simulationResult) {
	for (StatisticsGatherer* statGatherer : m_stats_gatherers) {
		statGatherer->AcquireResult(simulationResult);
	}
}

_StatisticalInfoTable CompositeStatisticsGatherer::GetStatisticalInfo() const {
	/* TODO
	 * It works but there are expensive copies to flatten all the vectors into a single vector!!
	 * In order to avoid copying vectors all in a single vector, which must have contigous elements,
	 * the _StatisticalInfoTable type could be changed from vector<_StatisticalInfo>
	 * to vector<unique_ptr<_StatisticalInfo>>: in such a way move semantics could be exploited since
	 * the original vectors in _StatisticalInfo objects could just have their ownership moved without
	 * the need to copy them in continous positions (just the pointers to them inside vector<unique_ptr<>>
	 * would be in contigous memory addresses).
	 */
	_StatisticalInfoTable flattenedInfoTable;
	for (StatisticsGatherer* statGatherer : m_stats_gatherers) {
		// TODO this vector (which could be huge for FullSampleGatherers) is copied!!
		_StatisticalInfoTable infoTableToInclude = statGatherer->GetStatisticalInfo();
		flattenedInfoTable.insert(
			flattenedInfoTable.end(),
			infoTableToInclude.begin(),
			infoTableToInclude.end()
		);
	}
	
	return flattenedInfoTable;
}
