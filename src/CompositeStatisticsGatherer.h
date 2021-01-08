#ifndef _COMPOSITE_STATISTICS_GATHERER_H_
#define _COMPOSITE_STATISTICS_GATHERER_H_

#include "StatisticsGatherer.h"

class CompositeStatisticsGatherer : public StatisticsGatherer {
	public:
		CompositeStatisticsGatherer* AddChildStatGatherer(std::unique_ptr<StatisticsGatherer> statGatherer);

		void AcquireResult(double simulationResult) override;

		_StatisticalInfoTable GetStatisticalInfo() const override;		

	private:
		std::vector<std::unique_ptr<StatisticsGatherer>> m_stats_gatherers;
};

#endif
