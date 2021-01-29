#ifndef _JSON_WRITER_H_
#define _JSON_WRITER_H_

#define OUTPUT_FOLDER "output"
#define BASE_FILE_NAME "statResults"

#include <string>

#include "rapidjson/document.h"

#include "result/PricingResults.h"

using _StatisticalInfo = std::pair<std::string, std::vector<double>>;
using _StatisticalInfoTable = std::vector<_StatisticalInfo>;

class JSONWriter {
	public:
		static void WriteResultsInfoTableToFile(
			const _StatisticalInfoTable& infoTable,
			std::string additionalFileLabel=""
		);

		static std::string SerializePricingResultsToString(const PricingResults& results, bool shouldIndent = true);

	private:
		static void FillSinglePriceGreekResult(
			rapidjson::Value& valueToFill,
			const PricingResults::SinglePriceGreekResult& resultComponent,
			rapidjson::Document& doc
		);

		static std::string SerializeDocToString(rapidjson::Document& doc, bool shouldIndent = true);

		static void SerializeDocToFile(rapidjson::Document& doc, const std::string& fullFileName, bool shouldIndent = true);
};

#endif
