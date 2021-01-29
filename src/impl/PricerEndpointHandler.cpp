#include "PricerEndpointHandler.h"

#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "derivative/Derivative.h"
#include "mkt-data/Discounter.h"
#include "mkt-data/FlatMarketData.h"
#include "path-generator/GBMPathGenerator.h"

#include "io/JSONReader.h"
#include "io/JSONWriter.h"

#include "montecarlo/MonteCarloSettings.h"
#include "montecarlo/MonteCarloHandler.h"
#include "payoff/PayoffFactory.h"
#include "variate-generator/UniVariateNumbersGenerator.h"
#include "variate-generator/UniVariateNumbersGeneratorFactory.h"
#include "statistics-gatherer/CompositeStatisticsGatherer.h"
#include "statistics-gatherer/FullSampleGatherer.h"
#include "statistics-gatherer/MomentsEvaluator.h"
#include "statistics-gatherer/StatisticsGatherer.h"

#include "statistics-gatherer/PricingResultsGatherer.h"

#include "result/PricingResults.h"

#include "util/Utils.h"

// TODO use unsigned long instead?
#ifndef _DATE_DEF_
#define _DATE_DEF_
using _Date = long;
#endif

// The evaluation date ("today")
constexpr _Date VALUE_DATE = 0;

// Average trading days per year.
// (the average number of trading days per year from 1990 to 2018 has been 251.86 for NYSE and NASDAQ)
constexpr int TRAD_DAYS_PER_YEAR = 252;

std::string PricerEndpointHandler::EvaluateBlack(
	const std::string& flatMktDataJSONString,
	const std::string& mcSettingsJSONString,
	const std::string& productJSONString
) {
	std::unique_ptr<Derivative> derivativePtr = JSONReader::ReadProduct(productJSONString);
	FlatMarketData flatMktData = JSONReader::ReadFlatMarketData(flatMktDataJSONString);
	MonteCarloSettings mcSettings = JSONReader::ReadMonteCarloSettings(mcSettingsJSONString);

	const unsigned int nThreads = mcSettings.m_n_threads;
	const size_t nMCSimulations = mcSettings.m_num_simulations;

	double r = flatMktData.m_rf_rate;
	double vola = flatMktData.m_vola;

	const double discountFactor = Discounter::Discount(VALUE_DATE, derivativePtr->m_expiry_date, r);

	// TODO TMP Print some inputs to check the consistency of final results
	std::cout << "\n" << "Time to expiration (days): " << derivativePtr->m_expiry_date - derivativePtr->m_issue_date << "\n";
	std::cout << "Underlying price: " << derivativePtr->m_underlying.GetReferencePrice() << "\n";
	// from here onwards, doubles are printed with 15 significant digits
	std::cout << std::setprecision(15);
	std::cout << "Discount factor: " << discountFactor << "\n";
	std::cout << "Yearly risk free rate: " << r * TRAD_DAYS_PER_YEAR << "\n";
	std::cout << "Yearly volatility: " << vola * std::sqrt(TRAD_DAYS_PER_YEAR) << "\n";
	std::cout << "(Note: assumed " << TRAD_DAYS_PER_YEAR << " trading days per year)" << "\n\n";
	std::cout << "Number of MonteCarlo scenarios: " << mcSettings.m_num_simulations << "\n\n";

	/*
	 * Payoff function
	 */
	auto payoff = PayoffFactory::GetInstance().CreatePayoff(
		derivativePtr->m_payoff_id,
		derivativePtr.get(),
		std::vector<double>(1, discountFactor)
	);
	std::vector<_Date> payoffObservations = payoff->m_flattened_observation_dates;

	/*
	 * Random numbers generators and stochastic process generator
	 */
	const bool shouldApplyAntitheticWrapper = (mcSettings.m_variance_reduction == VarianceReduction::ANTITHETIC);

	auto variateNumbersGenerator = UniVariateNumbersGeneratorFactory::GetInstance().CreateVariateGenerator(
		mcSettings.m_variate_generator_type,
		shouldApplyAntitheticWrapper,
		payoffObservations.size(),
		mcSettings.m_seed
	);

	std::unique_ptr<StochasticPathGenerator> geomBrownMotionGenerator = std::make_unique<GBMPathGenerator>(
		payoffObservations,
		(derivativePtr->m_underlying).GetReferencePrice(),
		mcSettings.m_greeks_settings.m_spot_relative_shift,
		r, vola,
		std::move(variateNumbersGenerator)
	);

	/*
	 * Statistics gatherers
	 * TODO add settings to customize statistics gatherers and handle their construction
	 */
	// two moments: for mean and std dev
	// TODO: now hardcoded, this parameter should come from the outside world
 	const unsigned int nMoments = 2;
	PricingResultsGatherer pricingResultsGatherer{nMoments};

	std::vector<std::unique_ptr<PricingResultsGatherer>> resultsGatherersPerThread;
	for (unsigned int i = 0; i < nThreads; ++i) {
		resultsGatherersPerThread.push_back(pricingResultsGatherer.Clone());
	}

	/*
	 * Launch MonteCarlo
	 */
	MonteCarloHandler mcHandler{
		nMCSimulations,
		nThreads,
		std::move(geomBrownMotionGenerator),
		std::move(payoff),
		resultsGatherersPerThread
	};
	// join until MC machinery stops
	mcHandler.Run();

	/*
	 * Merge results coming from each thread
	 */
	PricingResults results = Utils::ExtractPricingResultsFromGatherers(
		resultsGatherersPerThread,
		nThreads,
		nMoments,
		// divided by two, since two consecutive runs are already averaged in MonteCarloEngine
		nMCSimulations / 2
	);

	return JSONWriter::SerializePricingResultsToString(results);
}
