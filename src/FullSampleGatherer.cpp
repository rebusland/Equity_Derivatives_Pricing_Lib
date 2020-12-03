#include "FullSampleGatherer.h"

_StatisticalInfoTable FullSampleGatherer::GetStatisticalInfo() const {
	_StatisticalInfo samples("Full data", std::move(m_samples));
	return _StatisticalInfoTable(1, std::move(samples));
}
