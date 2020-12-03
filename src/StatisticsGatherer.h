#ifndef _STATISTICS_GATHERER_H_
#define _STATISTICS_GATHERER_H_

#include <iostream>
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

		static void PrintStatisticalInfoTable(const _StatisticalInfoTable& infoTable) {
			std::cout << "\n\n******* Statistics Info Table *******\n\n";

			for (const _StatisticalInfo& singleInfo : infoTable) {
				// each singleInfo is printed on a row
				std::cout << std::endl << singleInfo.first << ": ";
				for (double statMeasure : singleInfo.second) {
					std::cout << statMeasure << " ";
				}
			}
			std::cout << "\n\n*************************************\n\n";
		}

		/**
		 * TODO add method to dump statistics table to external file (and or Excel)
		 */
		 // static void DumpStatisticsInfoTableToFile(const _StatisticalInfoTable&)

		virtual ~StatisticsGatherer(){}
};

#endif
