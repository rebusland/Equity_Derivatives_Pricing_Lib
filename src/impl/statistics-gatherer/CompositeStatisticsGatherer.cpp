#include "statistics-gatherer/CompositeStatisticsGatherer.h"

#include <algorithm>

CompositeStatisticsGatherer* CompositeStatisticsGatherer::AddChildStatGatherer(std::unique_ptr<StatisticsGatherer> statGatherer) {
	m_stats_gatherers.push_back(std::move(statGatherer));
	return this;
}

void CompositeStatisticsGatherer::AcquireResult(double simulationResult) {
	for (const std::unique_ptr<StatisticsGatherer>& statGatherer : m_stats_gatherers) {
		statGatherer->AcquireResult(simulationResult);
	}
}

_StatisticalInfoTable CompositeStatisticsGatherer::GetStatisticalInfo() const {
	_StatisticalInfoTable compositeInfoTable;
	// Each unique pointer is moved to the "composite" vector.
	// Hopefully, this saved a lot of copies! (FullSampleGatherer's results vector could be pretty big!)
	for (const std::unique_ptr<StatisticsGatherer>& statGatherer : m_stats_gatherers) {
		auto infoToAdd = statGatherer->GetStatisticalInfo(); // please Lord, let RVO work!
		std::move(infoToAdd.begin(), infoToAdd.end(), std::back_inserter(compositeInfoTable));
	}
	return compositeInfoTable;
}

std::unique_ptr<StatisticsGatherer> CompositeStatisticsGatherer::clone() const {
	std::unique_ptr<CompositeStatisticsGatherer> compositeClone(new CompositeStatisticsGatherer());
	for (const auto& gatherer: m_stats_gatherers) {
		compositeClone->AddChildStatGatherer(gatherer->clone());
	}
	return compositeClone;
}
