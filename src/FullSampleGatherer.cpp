#include "FullSampleGatherer.h"

_StatisticalInfoTable FullSampleGatherer::GetStatisticalInfo() const {
	// in this way, everything should be moved (not copied)
	_StatisticalInfoTable fullSampleTable;
	fullSampleTable.push_back(std::make_unique<_StatisticalInfo>("Full data", std::move(m_samples)));
	return fullSampleTable;
}
