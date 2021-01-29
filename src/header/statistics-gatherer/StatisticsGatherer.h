#ifndef _STATISTICS_GATHERER_H_
#define _STATISTICS_GATHERER_H_

#include <memory>
#include <string>
#include <utility>
#include <vector>

/**
 * A flexible way to return multiple statistical measures.
 * Inside the pair, string is the name of the statistics and the vector contains it values.
 */
using _StatisticalInfo = std::pair<std::string, std::vector<double>>;
using _StatisticalInfoTable = std::vector<_StatisticalInfo>;

class StatisticsGatherer {
	public:
		/**
		 * Acquires a new single input to update the statistics.
		 */
		virtual void AcquireResult(double simulationResult) = 0;

		/**
		 * Returns the statistical info gathered and produced so far.
		 */
		virtual _StatisticalInfoTable GetStatisticalInfo() const = 0;

		/**
		 * Virtual constructor pattern (i.e. polimorphic cloning)
		 */
		virtual std::unique_ptr<StatisticsGatherer> clone() const = 0;

		static void PrintStatisticalInfoTable(const _StatisticalInfoTable& infoTable);

		static void DownloadStatisticalInfoTable(
			const _StatisticalInfoTable& infoTable, 
			std::string additionalFileLabel=""
		);

		virtual ~StatisticsGatherer(){}
};

#endif
