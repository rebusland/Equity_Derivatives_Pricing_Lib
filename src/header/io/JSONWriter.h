#ifndef _JSON_WRITER_H_
#define _JSON_WRITER_H_

#define OUTPUT_FOLDER "output"
#define BASE_FILE_NAME "statResults"

#include <fstream>
#include <string>

#include "rapidjson/document.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include "result/PricingResults.h"

using _StatisticalInfo = std::pair<std::string, std::vector<double>>;
using _StatisticalInfoTable = std::vector<_StatisticalInfo>;

class JSONWriter {
	public:
		static void WriteResultsInfoTableToFile(
			const _StatisticalInfoTable& infoTable,
			std::string additionalFileLabel=""
		) {
			rapidjson::Document doc;
			doc.SetObject();
			for (const auto& statInfo : infoTable) {
				rapidjson::Value statResultArray(rapidjson::kArrayType);

				// a bit annoying..why can't I just assign the vector to the rapidjson array?
				for (const double singleResult : statInfo.second) {
					statResultArray.PushBack(singleResult, doc.GetAllocator());
				}

				/* how the key name is set is awful, but only in this way is possible to set
				 * a non-literal as the key name. Indeed, this is the solution proposed in the
				 * RapidJson documentation.
				 */
				rapidjson::Value key((statInfo.first).c_str(), doc.GetAllocator());
				doc.AddMember(key, statResultArray, doc.GetAllocator());
			}

			std::string fullFileName = OUTPUT_FOLDER "/" BASE_FILE_NAME + additionalFileLabel + ".json";
			SerializeDocToFile(doc, fullFileName);
		}

		static std::string SerializePricingResultsToString(const PricingResults& results, bool shouldIndent = true) {
			rapidjson::Document doc;
			doc.SetObject();

			doc.AddMember("fairPrice", results.m_fair_price, doc.GetAllocator());
			doc.AddMember("monteCarloError", results.m_montecarlo_error, doc.GetAllocator());

			rapidjson::Value momentsArray(rapidjson::kArrayType);
			// a bit annoying..why can't I just assign the vector to the rapidjson array?
			for (const double moment : results.m_moments) {
				momentsArray.PushBack(moment, doc.GetAllocator());
			}
			doc.AddMember("moments", momentsArray, doc.GetAllocator());

			return SerializeDocToString(doc, shouldIndent);
		}

	private:
		static std::string SerializeDocToString(rapidjson::Document& doc, bool shouldIndent = true) {
			rapidjson::StringBuffer buffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
			if (shouldIndent) {
				writer.SetIndent('\t', 1); // indent using tabs
			}
			doc.Accept(writer);
			return buffer.GetString();
		}

		static void SerializeDocToFile(rapidjson::Document& doc, const std::string& fullFileName, bool shouldIndent = true) {
			std::ofstream ofs(fullFileName.c_str());
			rapidjson::OStreamWrapper osw(ofs);

			rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
			if (shouldIndent) {
				writer.SetIndent('\t', 1); // indent using tabs
			}
			doc.Accept(writer);
		}
};

#endif
