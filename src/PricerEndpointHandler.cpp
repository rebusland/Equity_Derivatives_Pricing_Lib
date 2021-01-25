#include "PricerEndpointHandler.h"

#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "Derivative.h"
#include "Discounter.h"
#include "GBMPathGenerator.h"
#include "JSONReader.h"
#include "MonteCarloSettings.h"
#include "MonteCarloHandler.h"
#include "PayoffFactory.h"
#include "UniVariateNumbersGenerator.h"
#include "UniVariateNumbersGeneratorFactory.h"

// TODO use unsigned long instead?
using _Date = long;

// The evaluation date ("today")
constexpr _Date VALUE_DATE = 0;

// Average trading days per year.
// (the average number of trading days per year from 1990 to 2018 has been 251.86 for NYSE and NASDAQ)
constexpr int TRAD_DAYS_PER_YEAR = 252;

void PricerEndpointHandler::EvaluateBlack() {
	std::unique_ptr<Derivative> derivativePtr = JSONReader::ReadProduct();

	auto flatMktData = JSONReader::ReadFlatMarketData();
	double r = flatMktData.m_rf_rate;
	double vola = flatMktData.m_vola;

	const double discountFactor = Discounter::Discount(VALUE_DATE, derivativePtr->m_expiry_date, r);

	MonteCarloSettings mcSettings = JSONReader::ReadMonteCarloSettings();
	const double seed = mcSettings.m_seed;
	const VariateGeneratorEnum generatorType = mcSettings.m_variate_generator_type;
	const size_t NUM_SIMUL = mcSettings.m_num_simulations;
	const unsigned int NUM_THREAD = mcSettings.m_n_threads;
	const bool shouldApplyAntitheticWrapper = (mcSettings.m_variance_reduction == VarianceReduction::ANTITHETIC);

	// TMP Print some inputs to check the consistency of final results
	std::cout << "\n" << "Time to expiration (days): " << derivativePtr->m_expiry_date - derivativePtr->m_issue_date << "\n";
	std::cout << "Underlying price: " << derivativePtr->m_underlying.GetReferencePrice() << "\n";
	// from here onwards, doubles are printed with 15 significant digits
	std::cout << std::setprecision(15);
	std::cout << "Discount factor: " << discountFactor << "\n";
	std::cout << "Yearly risk free rate: " << r * TRAD_DAYS_PER_YEAR << "\n";
	std::cout << "Yearly volatility: " << vola * std::sqrt(TRAD_DAYS_PER_YEAR) << "\n";
	std::cout << "(Note: assumed " << TRAD_DAYS_PER_YEAR << " trading days per year)" << "\n\n";
	std::cout << "Number of MonteCarlo scenarios: " << NUM_SIMUL << "\n\n";

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
	auto variateNumbersGenerator = UniVariateNumbersGeneratorFactory::GetInstance().CreateVariateGenerator(
		generatorType,
		shouldApplyAntitheticWrapper,
		payoffObservations.size(),
		seed
	);

	std::unique_ptr<StochasticPathGenerator> geomBrownMotionGenerator = std::make_unique<GBMPathGenerator>(
		payoffObservations,
		(derivativePtr->m_underlying).GetReferencePrice(),
		r, vola,
		std::move(variateNumbersGenerator)
	);

	MonteCarloHandler mcHandler{
		NUM_SIMUL,
		NUM_THREAD,
		std::move(geomBrownMotionGenerator),
		std::move(payoff)
	};
	mcHandler.Run();
}
