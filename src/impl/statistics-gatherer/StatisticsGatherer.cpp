#include "statistics-gatherer/StatisticsGatherer.h"

#include <iostream>

#include "io/JSONWriter.h"

void StatisticsGatherer::PrintStatisticalInfoTable(const _StatisticalInfoTable& infoTable) {
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
 * TODO 
 *  add enum to determine where to dump the statistics table (JSON and/or Excel)?
 */
void StatisticsGatherer::DownloadStatisticalInfoTable(
	const _StatisticalInfoTable& infoTable, 
	std::string additionalFileLabel
) {
	JSONWriter::WriteResultsInfoTableToFile(infoTable, additionalFileLabel);
}
