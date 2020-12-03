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
	_StatisticalInfoTable compositeInfoTable;
	// Each unique pointer is moved to the "composite" vector.
	// Hopefully, this saved a lot of copies! (FullSampleGatherer's results vector could be pretty big!)
	for (StatisticsGatherer* statGatherer : m_stats_gatherers) {
		auto infoToAdd = statGatherer->GetStatisticalInfo(); // please Lord, let RVO work!
		std::move(infoToAdd.begin(), infoToAdd.end(), std::back_inserter(compositeInfoTable));
	}
	return compositeInfoTable;
}
