#include "io/JSONReader.h"

#include "util/ProjectException.h"

FlatMarketData JSONReader::ReadFlatMarketData(const std::string& flatMktDataString) {
	rapidjson::Document doc;
	ParseString(doc, flatMktDataString);

	return FlatMarketData{doc["rf_rate"].GetDouble(), doc["volatility"].GetDouble()};
}

MonteCarloSettings JSONReader::ReadMonteCarloSettings(const std::string& mcSettingsString) {
	rapidjson::Document doc;
	ParseString(doc, mcSettingsString);

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

	std::string generatorLabel = doc["variateGenerator"].GetString();
	VariateGeneratorEnum generatorEnum;
	if (generatorLabel == "STD_GAUSS") {
		generatorEnum = VariateGeneratorEnum::STD_GAUSS;
	} else {
		THROW_PROJECT_EXCEPTION("Invalid variate generator type: " + generatorLabel);
	}

	const auto& greeksSettingsJSON = doc["greeks"];
	MonteCarloSettings::GreeksSettings greeksSettings{
		greeksSettingsJSON["spot-relative-shift"].GetDouble()
	};

	return MonteCarloSettings{
		generatorEnum,
		doc["seed"].GetDouble(),
		simulScheduler,
		doc["nThreads"].GetUint(),
		varianceReduction,
		doc["nSimulations"].GetUint64(),
		greeksSettings
	};
}

std::unique_ptr<Derivative> JSONReader::ReadProduct(const std::string& productString) {
	rapidjson::Document doc;
	ParseString(doc, productString);

	if (doc.HasMember("asianOption")) {
		return ReadAsianOption(doc);

	} else if (doc.HasMember("vanillaOption")) {
		return ReadVanillaOption(doc);

	} else {
		THROW_PROJECT_EXCEPTION("Missing specific type in product's json");
	}
}

std::unique_ptr<Derivative> JSONReader::ReadDerivative(const rapidjson::Document& doc) {
	std::string alias = doc["underlying"]["alias"].GetString();
	double refPrice = doc["underlying"]["spotPrice"].GetDouble();
	double spotPrice = doc["underlying"]["refPrice"].GetDouble();

	_Date issueDate = doc["issueDate"].GetInt();
	_Date expiryDate = doc["expiryDate"].GetInt();

	std::string payoffId = doc["payoffId"].GetString();

	return std::make_unique<Derivative>(
		Underlying{alias, refPrice, spotPrice},
		issueDate,
		expiryDate,
		payoffId
	);
}

std::unique_ptr<VanillaOption> JSONReader::ReadVanillaOption(const rapidjson::Document& doc) {
	std::unique_ptr<Derivative> derivativePart = ReadDerivative(doc);

	const auto& vanillaOptionJSON = doc["vanillaOption"];

	// CALL or PUT
	CallPut callPut = ReadCallPut(vanillaOptionJSON["callPut"].GetString());
	double strike = vanillaOptionJSON["strike"].GetDouble();

	return std::make_unique<VanillaOption>(
		derivativePart->m_underlying,
		derivativePart->m_issue_date,
		derivativePart->m_expiry_date,
		derivativePart->m_payoff_id,
		callPut,
		strike
	);
}

std::unique_ptr<AsianOption> JSONReader::ReadAsianOption(const rapidjson::Document& doc) {
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
		derivativePart->m_underlying,
		derivativePart->m_issue_date,
		derivativePart->m_expiry_date,
		derivativePart->m_payoff_id,
		callPut,
		strikeFixingDates,
		avgTypeStrike,
		priceFixingDates,
		avgTypePrice
	);
}

void JSONReader::ParseString(rapidjson::Document& doc, const std::string& jsonStr) {
	doc.Parse(jsonStr.c_str());

	if (doc.HasParseError()) {
		THROW_PROJECT_EXCEPTION("Error when parsing json string: " + jsonStr);
	}
}

CallPut JSONReader::ReadCallPut(const std::string& callPutString) {
	if (callPutString == "CALL") {
		return CallPut::CALL;
	} else if (callPutString == "PUT") {
		return CallPut::PUT;
	} else {
		THROW_PROJECT_EXCEPTION("Invalid option side: " + callPutString);
	}
}

AvgType JSONReader::ReadAvgType(const std::string& avgTypeString) {
	if (avgTypeString == "ARITHMETIC") {
		return AvgType::ARITHMETIC;
	} else if (avgTypeString == "GEOMETRIC") {
		return AvgType::GEOMETRIC;
	} else {
		THROW_PROJECT_EXCEPTION("Invalid averaging type: " + avgTypeString);
	}
}

