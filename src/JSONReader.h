#ifndef _JSON_READER_H_
#define _JSON_READER_H_

#include <fstream>

// RAPIDJSON
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"

// datamodels
#include "AsianOption.h"
#include "FlatMarketData.h"
#include "MonteCarloSettings.h"

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

		/**
		 * TODO
		 * Create a more generic reader ReadProduct() which gathers all the info pertaining the
		 * "Derivative" part of the product (issueDate, underlying etc.) and calls a specific reader
		 * depending on the type of payoff: this method should return a polymorphic Derivative object
		 * i.e. a pointer or smart pointer.
		 */
		static AsianOption ReadAsianOption() {
			std::ifstream ifs("input/product.json");
			rapidjson::IStreamWrapper isw(ifs);
			rapidjson::Document doc;
			doc.ParseStream(isw);

			std::string alias = doc["underlying"]["alias"].GetString();
			double refPrice = doc["underlying"]["spotPrice"].GetDouble();
			double spotPrice = doc["underlying"]["refPrice"].GetDouble();

			Underlying underlying{alias, refPrice, spotPrice};

			_Date issueDate = doc["issueDate"].GetInt();
			_Date expiryDate = doc["expiryDate"].GetInt();

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

			return 	AsianOption{
				underlying, issueDate, expiryDate,
				callPut, strikeFixingDates, avgTypeStrike,
				priceFixingDates, avgTypePrice
			};
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
};

#endif
