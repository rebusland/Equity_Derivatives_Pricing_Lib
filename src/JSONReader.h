#ifndef _JSON_READER_H_
#define _JSON_READER_H_

#include <memory>

// RAPIDJSON
#include "rapidjson/document.h"

// datamodels
#include "AsianOption.h"
#include "FlatMarketData.h"
#include "MonteCarloSettings.h"
#include "VanillaOption.h"

class JSONReader {
	public:
		static FlatMarketData ReadFlatMarketData(const std::string& flatMktDataString);

		static MonteCarloSettings ReadMonteCarloSettings(const std::string& mcSettingsString);

		static std::unique_ptr<Derivative> ReadProduct(const std::string& productString);

		static std::unique_ptr<Derivative> ReadDerivative(const rapidjson::Document& doc);

		static std::unique_ptr<VanillaOption> ReadVanillaOption(const rapidjson::Document& doc);

		static std::unique_ptr<AsianOption> ReadAsianOption(const rapidjson::Document& doc);

	private:
		static void ParseString(rapidjson::Document& doc, const std::string& jsonStr);

		static CallPut ReadCallPut(const std::string& callPutString);

		static AvgType ReadAvgType(const std::string& avgTypeString);
};

#endif
