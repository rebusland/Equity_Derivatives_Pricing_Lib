#ifndef _JSON_READER_H_
#define _JSON_READER_H_

#include <fstream>
#include <memory>

// RAPIDJSON
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

// datamodels
#include "AsianOption.h"
#include "FlatMarketData.h"
#include "MonteCarloSettings.h"
#include "VanillaOption.h"

#include "ProjectException.h"

class JSONReader {
	public:
		static CallPut ReadCallPut(const std::string& callPutString) {
			if (callPutString == "CALL") {
				return CallPut::CALL;
			} else if (callPutString == "PUT") {
				return CallPut::PUT;
			} else {
				THROW_PROJECT_EXCEPTION("Invalid option side: " + callPutString);
			}
		}

		static AvgType ReadAvgType(const std::string& avgTypeString) {
			if (avgTypeString == "ARITHMETIC") {
				return AvgType::ARITHMETIC;
			} else if (avgTypeString == "GEOMETRIC") {
				return AvgType::GEOMETRIC;
			} else {
				THROW_PROJECT_EXCEPTION("Invalid averaging type: " + avgTypeString);
			}
		}

		static FlatMarketData ReadFlatMarketData() {
			std::ifstream ifs("input/flatMarketData.json");
			rapidjson::IStreamWrapper isw(ifs);
			rapidjson::Document doc;
			doc.ParseStream(isw);

			return FlatMarketData{doc["rf_rate"].GetDouble(), doc["volatility"].GetDouble()};
		}

		static MonteCarloSettings ReadMonteCarloSettings() {
			std::ifstream ifs("input/monteCarloSettings.json");
			rapidjson::IStreamWrapper isw(ifs);
			rapidjson::Document doc;
			doc.ParseStream(isw);

			std::string simulSchedulString = doc["simulationScheduler"].GetString();
			SimulationScheduler simulScheduler;

			if (simulSchedulString == "SEQUENTIAL") {
				simulScheduler = SimulationScheduler::SEQUENTIAL;
			} else if (simulSchedulString == "MULTITHREAD") {
				simulScheduler = SimulationScheduler::SEQUENTIAL;
			} else {
				THROW_PROJECT_EXCEPTION("Invalid simulation scheduler: " + simulSchedulString + " for MonteCarloSettings.");
			}

			std::string varianceReductionString = doc["varianceReduction"].GetString();
			VarianceReduction varianceReduction;

			if (varianceReductionString == "NONE") {
				varianceReduction = VarianceReduction::NONE;
			} else if (varianceReductionString == "ANTITHETIC") {
				varianceReduction = VarianceReduction::ANTITHETIC;
			} else if (varianceReductionString == "CONTROL") {
				varianceReduction = VarianceReduction::CONTROL;
			} else {
				THROW_PROJECT_EXCEPTION("Invalid variance reduction technique: " + varianceReductionString);
			}

			return MonteCarloSettings{
				simulScheduler,
				varianceReduction,
				doc["nSimulations"].GetUint64()
			};
		}

		static std::unique_ptr<Derivative> ReadProduct() {
			std::ifstream ifs("input/product.json");
			rapidjson::IStreamWrapper isw(ifs);
			rapidjson::Document doc;
			doc.ParseStream(isw);

			if (doc.HasMember("asianOption")) {
				return ReadAsianOption(doc);

			} else if (doc.HasMember("vanillaOption")) {
				return ReadVanillaOption(doc);

			} else {
				THROW_PROJECT_EXCEPTION("Missing specif type in product's json");
			}
		}

		static std::unique_ptr<Derivative> ReadDerivative(const rapidjson::Document& doc) {
			std::string alias = doc["underlying"]["alias"].GetString();
			double refPrice = doc["underlying"]["spotPrice"].GetDouble();
			double spotPrice = doc["underlying"]["refPrice"].GetDouble();

			_Date issueDate = doc["issueDate"].GetInt();
			_Date expiryDate = doc["expiryDate"].GetInt();

			return std::make_unique<Derivative>(Underlying{alias, refPrice, spotPrice}, issueDate, expiryDate);
		}

		static std::unique_ptr<VanillaOption> ReadVanillaOption(const rapidjson::Document& doc) {
			std::unique_ptr<Derivative> derivativePart = ReadDerivative(doc);

			const auto& vanillaOptionJSON = doc["vanillaOption"];

			// CALL or PUT
			CallPut callPut = ReadCallPut(vanillaOptionJSON["callPut"].GetString());
			double strike = vanillaOptionJSON["strike"].GetDouble();

			return std::make_unique<VanillaOption>(
				derivativePart->m_underlying, derivativePart->m_issue_date, derivativePart->m_expiry_date,
				callPut, strike
			);
		}

		static std::unique_ptr<AsianOption> ReadAsianOption(const rapidjson::Document& doc) {
			std::unique_ptr<Derivative> derivativePart = ReadDerivative(doc);

			const auto& asianOptionJSON = doc["asianOption"];

			// CALL or PUT
			CallPut callPut = ReadCallPut(asianOptionJSON["callPut"].GetString());

			// STRIKE FIXING DATES
			std::vector<_Date> strikeFixingDates;
			auto& strikeDatesVal = asianOptionJSON["strikeFixingDates"];
			for (const auto& dateVal : strikeDatesVal.GetArray()) {
				strikeFixingDates.push_back(dateVal.GetInt());
			}

			// PRICE FIXING DATES
			std::vector<_Date> priceFixingDates;
			auto& priceDatesVal = asianOptionJSON["priceFixingDates"];
			for (const auto& dateVal : priceDatesVal.GetArray()) {
				priceFixingDates.push_back(dateVal.GetInt());
			}

			// AVG TYPE STRIKE
			AvgType avgTypeStrike = ReadAvgType(asianOptionJSON["avgTypeStrike"].GetString());

			// AVG TYPE PRICE
			AvgType avgTypePrice = ReadAvgType(asianOptionJSON["avgTypePrice"].GetString());

			// TODO create constructor to accept a whole derivative objec
			return std::make_unique<AsianOption>(
				derivativePart->m_underlying, derivativePart->m_issue_date, derivativePart->m_expiry_date,
				callPut, strikeFixingDates, avgTypeStrike,
				priceFixingDates, avgTypePrice
			);
		}
};

#endif
