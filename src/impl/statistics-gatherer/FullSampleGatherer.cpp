#include "statistics-gatherer/FullSampleGatherer.h"

_StatisticalInfoTable FullSampleGatherer::GetStatisticalInfo() const {
	// in this way, everything should be moved (not copied)
	_StatisticalInfoTable fullSampleTable;
	fullSampleTable.push_back(_StatisticalInfo(FULL_DATA_STRING, std::move(m_samples)));
	return fullSampleTable;
}

std::unique_ptr<StatisticsGatherer> FullSampleGatherer::clone() const {
	return std::make_unique<FullSampleGatherer>(*this);
}
