#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "AntitheticWrapperUniVariateGenerator.h"
#include "AsianOption.h"
#include "AsianPayoff.h"
#include "CompositeStatisticsGatherer.h"
#include "Discounter.h"
#include "FullSampleGatherer.h"
#include "FunctionalWrapperUniVariateGenerator.h"
#include "GBMPathGenerator.h"
#include "JSONReader.h"
#include "MomentsEvaluator.h"
#include "MonteCarloEngine.h"
#include "MonteCarloSettings.h"
#include "PlainVanillaPayoff.h"
#include "Timer.h"
#include "Underlying.h"
#include "UniVariateNumbersGenerator.h"

// TODO use unsigned long instead?
using _Date = long;
using _StatePayoffFunc = std::function<double (double)>;
using _PathDependentPayoffFunc = std::function<double (const std::vector<double>&)>;

// TODO IMPORTANT: we assume that the minimum time step is a single day.
// This is the time step used by default in the underlying SDE's equation, generating the MC paths.
constexpr double MIN_TIME_STEP = 1;

// The evaluation date ("today")
constexpr _Date VALUE_DATE = 0;

// Average trading days per year.
// (the average number of trading days per year from 1990 to 2018 has been 251.86 for NYSE and NASDAQ)
constexpr int TRAD_DAYS_PER_YEAR = 252;

/**
 * TODO:
 *  - implement the Builder pattern for constructing complex objects!!
 *  - standardize project structures in packages/folders
 *  - add more modularity to the code! Start splitting this main
 *  - standardize and check memory management accross code entities, namely use smart pointers as much as possible.
 */
int main() {
	_TIMER_MILLIS_(myTimer);

	AsianOption asianOption = JSONReader::ReadAsianOption();

	auto flatMktData = JSONReader::ReadFlatMarketData();
	double r = flatMktData.m_rf_rate;
	double vola = flatMktData.m_vola;

	const double discountFactor = Discounter::Discount(VALUE_DATE, asianOption.m_expiry_date, r);

	MonteCarloSettings mcSettings = JSONReader::ReadMonteCarloSettings();
	const unsigned long long NUM_SIMUL = mcSettings.GetNumSimulations();

	// TMP Print some inputs to check the consistency of final results
	std::cout << "\n" << "Time to expiration (days): " << asianOption.m_expiry_date - asianOption.m_issue_date << "\n";
	std::cout << "Option side: " << (asianOption.m_call_put == CallPut::CALL ? "Call" : "Put") << "\n";
	std::cout << "Underlying price: " << asianOption.m_underlying.GetReferencePrice() << "\n";
	// from here onwards, doubles are printed with 15 significant digits
	std::cout << std::setprecision(15);
	std::cout << "Discount factor: " << discountFactor << "\n";
	std::cout << "Yearly risk free rate: " << r * TRAD_DAYS_PER_YEAR << "\n";
	std::cout << "Yearly volatility: " << vola * std::sqrt(TRAD_DAYS_PER_YEAR) << "\n";
	std::cout << "(Note: assumed " << TRAD_DAYS_PER_YEAR << " trading days per year)" << "\n\n";
	std::cout << "Number of MonteCarlo scenarios: " << NUM_SIMUL << "\n\n";

	/**
	 * TODO
	 *  - Random numbers engines (generators) in <random> header should be reproducible 
	 *    accross compilers; however, distributions seems not!
	 *    Should I write my own implementation of normal distribution? 
	 *	  Or implement the Box-Muller transformation? Or start exploiting the Boost library?
	 */
	// Park-Miller linear_congruential_engine
	std::minstd_rand uniformRng;
	// by default on creation is N(0, 1) as needed
	std::normal_distribution<double> normalDistr;
	auto normalVariateGeneratorFunc = std::bind(normalDistr, uniformRng);

	std::unique_ptr<UniVariateNumbersGenerator> normalVariateGenerator = 
		std::make_unique<FunctionalWrapperUniVariateGenerator>(2, normalVariateGeneratorFunc);
	std::unique_ptr<UniVariateNumbersGenerator> normalVariateGeneratorAntithetic = 
		std::make_unique<AntitheticWrapperUniVariateGenerator>(2, normalVariateGenerator->clone());

	std::unique_ptr<CompositeStatisticsGatherer> compositeStatGatherer(new CompositeStatisticsGatherer());
	compositeStatGatherer->AddChildStatGatherer(std::make_unique<MomentsEvaluator>(2));
	// ->AddChildStatGatherer(std::make_unique<FullSampleGatherer>);

	_StatePayoffFunc vanillaFunc;
	using std::placeholders::_1;
	/* 
	 * NB: the CPP standard states that:
	 * << The arguments to bind are copied or moved, and are never passed by reference unless
	 * wrapped in std::ref or std::cref>>
	 */
	if (asianOption.m_call_put == CallPut::CALL) {
		auto vanillaCall = PlainVanillaPayoff<CallPut::CALL>(10.0);
		vanillaFunc = std::bind(&PlainVanillaPayoff<CallPut::CALL>::operator(), vanillaCall, _1);

	} else {
		auto vanillaPut = PlainVanillaPayoff<CallPut::PUT>(10.0);
		vanillaFunc = std::bind(&PlainVanillaPayoff<CallPut::PUT>::operator(), vanillaPut, _1);
	}

	AsianPayoff asianPayoff{asianOption, discountFactor};
	const std::vector<_Date> payoffObservations = asianPayoff.m_flattened_observation_dates;

	_PathDependentPayoffFunc asianPayoffFunc = std::bind(&AsianPayoff::operator(), asianPayoff, _1);

	std::unique_ptr<StochasticPathGenerator> geomBrownMotionGenerator = std::make_unique<GBMPathGenerator>(
		payoffObservations,
		// std::vector<_Date>(1, asianOption.m_expiry_date),
		asianOption.m_underlying.GetReferencePrice(),
		r, vola,
		normalVariateGeneratorAntithetic->clone()
	);

	MonteCarloEngine<_PathDependentPayoffFunc> mcEngine{
		mcSettings,
		payoffObservations.size(),
		geomBrownMotionGenerator->clone(),
		asianPayoffFunc,
		compositeStatGatherer.get()
	};
	mcEngine();

/*
	MonteCarloEngine<_StatePayoffFunc> mcEngine{
		mcSettings,
		1,
		&geomBrownMotionGenerator,
		vanillaFunc,
		compositeStatGatherer
	};
	mcEngine.EvaluatePayoff();
*/

	const auto& fullInfoTable = compositeStatGatherer->GetStatisticalInfo();

	auto momentsInfoTable_it = std::find_if(
		fullInfoTable.cbegin(), fullInfoTable.cend(),
		[](const auto& infoTableUP) {return (infoTableUP->first == "Moments");}
	);
	std::vector<double> moments = (*momentsInfoTable_it)->second;

	const double finalPrice = moments[0];
	const double M2 = moments[1];

	// sigma/sqrt(n) = sqrt(M2 - M1^2) / sqrt(n) (NB: finalPrice == M1)
	const double stdDevMean = std::sqrt((M2 - finalPrice * finalPrice) / (0.5 * NUM_SIMUL - 1));

	std::cout << "Final MonteCarlo price: " << finalPrice << "\n";
	std::cout << "Std dev of the mean: " << stdDevMean << std::endl;

	// Print on console just the first moments of the simulation results
	StatisticsGatherer::PrintStatisticalInfoTable(fullInfoTable);

	// Write to files all the statistical info on the MonteCarlo routine
	// StatisticsGatherer::DownloadStatisticalInfoTable(fullInfoTable);
}
