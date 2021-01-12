#ifndef _JSON_WRITER_H_
#define _JSON_WRITER_H_

#define OUTPUT_FOLDER "output"
#define BASE_FILE_NAME "statResults"

#include <fstream>

#include "rapidjson/document.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

using _StatisticalInfo = std::pair<std::string, std::vector<double>>;
using _StatisticalInfoTable = std::vector<_StatisticalInfo>;

class JSONWriter {
	public:
		static void WriteResultsInfoTable(
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
			std::ofstream ofs(fullFileName.c_str());
			rapidjson::OStreamWrapper osw(ofs);

			rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
			writer.SetIndent('\t', 1); // indent using tabs
			doc.Accept(writer);
		}
};

#endif
