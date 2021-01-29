#include "io/JSONWriter.h"

#include <fstream>

#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

void JSONWriter::WriteResultsInfoTableToFile(
	const _StatisticalInfoTable& infoTable,
	std::string additionalFileLabel
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

std::string JSONWriter::SerializePricingResultsToString(const PricingResults& results, bool shouldIndent) {
	rapidjson::Document doc;
	doc.SetObject();

	// price results
	PricingResults::SinglePriceGreekResult priceResults = results.m_price_result;
	rapidjson::Value priceValue(rapidjson::kObjectType);
	FillSinglePriceGreekResult(priceValue, priceResults, doc);

	// delta results
	PricingResults::SinglePriceGreekResult deltaResults = results.m_delta_result;
	rapidjson::Value deltaValue(rapidjson::kObjectType);
	FillSinglePriceGreekResult(deltaValue, deltaResults, doc);

	// gamma results
	PricingResults::SinglePriceGreekResult gammaResults = results.m_gamma_result;
	rapidjson::Value gammaValue(rapidjson::kObjectType);
	FillSinglePriceGreekResult(gammaValue, gammaResults, doc);

	doc.AddMember("price", priceValue, doc.GetAllocator());
	doc.AddMember("delta", deltaValue, doc.GetAllocator());
	doc.AddMember("gamma", gammaValue, doc.GetAllocator());

	return SerializeDocToString(doc, shouldIndent);
}

void JSONWriter::FillSinglePriceGreekResult(
	rapidjson::Value& valueToFill,
	const PricingResults::SinglePriceGreekResult& resultComponent,
	rapidjson::Document& doc
) {
	rapidjson::Value momentsArray(rapidjson::kArrayType);
	// a bit annoying..why can't I just assign the vector to the rapidjson array?
	for(double moment : resultComponent.m_moments) {
		momentsArray.PushBack(moment, doc.GetAllocator());
	}
	valueToFill.AddMember("fair", resultComponent.m_mid, doc.GetAllocator());
	valueToFill.AddMember("monteCarloError", resultComponent.m_montecarlo_error, doc.GetAllocator());
	valueToFill.AddMember("moments", momentsArray, doc.GetAllocator());		
}

std::string JSONWriter::SerializeDocToString(rapidjson::Document& doc, bool shouldIndent) {
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	if (shouldIndent) {
		writer.SetIndent('\t', 1); // indent using tabs
	}
	doc.Accept(writer);
	return buffer.GetString();
}

void JSONWriter::SerializeDocToFile(rapidjson::Document& doc, const std::string& fullFileName, bool shouldIndent) {
	std::ofstream ofs(fullFileName.c_str());
	rapidjson::OStreamWrapper osw(ofs);

	rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
	if (shouldIndent) {
		writer.SetIndent('\t', 1); // indent using tabs
	}
	doc.Accept(writer);
}
