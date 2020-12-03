#ifndef _COMPOSITE_STATISTICS_GATHERER_H_
#define _COMPOSITE_STATISTICS_GATHERER_H_

#include "StatisticsGatherer.h"

#include <initializer_list>

class CompositeStatisticsGatherer : public StatisticsGatherer {
	public:
		CompositeStatisticsGatherer(std::initializer_list<StatisticsGatherer*> statsGatherers);

		void AcquireResult(double simulationResult) override;

		_StatisticalInfoTable GetStatisticalInfo() const override;		

	private:
		// TODO should I use unique_ptrs instead??
		std::vector<StatisticsGatherer*> m_stats_gatherers;
};

#endif
