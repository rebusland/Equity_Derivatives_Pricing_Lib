#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "AntitheticWrapperUniVariateGenerator.h"
#include "AsianOption.h"
#include "AsianPayoff.h"
#include "CompositeStatisticsGatherer.h"
#include "Discounter.h"
#include "FullSampleGatherer.h"
#include "FunctionalWrapperUniVariateGenerator.h"
#include "GaussianVariatesGenerator.h"
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

/*
 * IMPORTANT:
 * The same functional wrapper is used for both path dependent and "State"
 * (i.e. depending only on spot price at expiry) payoffs. Thus, when defining State payoffs
 * we have to artificially use a vector of 1 spot price in contrast to a scalar. This should not affect
 * the performance that much in evaluating the MonteCarlo scenarios, but it simplifies a lot the code.
 */
using _PayoffFunc = std::function<double (const std::vector<double>&)>;

// TODO IMPORTANT: we assume that the minimum time step is a single day.
// This is the time step used by default in the underlying SDE's equation, generating the MC paths.
constexpr double MIN_TIME_STEP = 1;

// The evaluation date ("today")
constexpr _Date VALUE_DATE = 0;

// Average trading days per year.
// (the average number of trading days per year from 1990 to 2018 has been 251.86 for NYSE and NASDAQ)
constexpr int TRAD_DAYS_PER_YEAR = 252;

constexpr unsigned int NUM_THREAD = 4;

/**
 * TODO:
 *  - implement the Builder pattern for constructing complex objects!!
 *  - standardize project structures in packages/folders
 *  - add more modularity to the code! Start splitting this main
 *  - standardize and check memory management accross code entities, namely use smart pointers as much as possible.
 */
int main() {
	_TIMER_MILLIS_(myTimer);

	std::unique_ptr<Derivative> derivativePtr = JSONReader::ReadProduct();
	AsianOption asianOption = *(dynamic_cast<AsianOption*>(derivativePtr.get()));

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

	/*
	 * Payoff function
	 */
	_PayoffFunc payoffFunc;
	using std::placeholders::_1;
	std::vector<_Date> payoffObservations;

	/*
	// european plain vanilla
	if (asianOption.m_call_put == CallPut::CALL) {
		auto vanillaCall = PlainVanillaPayoff<CallPut::CALL>(asianOption.m_expiry_date, 10.0);
		payoffFunc = std::bind(&PlainVanillaPayoff<CallPut::CALL>::operator(), vanillaCall, _1);
		payoffObservations = vanillaCall.m_flattened_observation_dates;

	} else {
		auto vanillaPut = PlainVanillaPayoff<CallPut::PUT>(asianOption.m_expiry_date, 10.0);
		payoffFunc = std::bind(&PlainVanillaPayoff<CallPut::PUT>::operator(), vanillaPut, _1);
		payoffObservations = vanillaPut.m_flattened_observation_dates;
	}
	*/

	// asian payoff
	AsianPayoff asianPayoff{asianOption, discountFactor};
	payoffObservations = asianPayoff.m_flattened_observation_dates;
	payoffFunc = std::bind(&AsianPayoff::operator(), asianPayoff, _1);

	const unsigned int payoffObsSize = payoffObservations.size();

	/*
	 * Random numbers generators and stochastic process generator
	 */
	std::unique_ptr<UniVariateNumbersGenerator> gaussianVariatesGenerator = 
		std::make_unique<GaussianVariatesGenerator>(payoffObsSize);

	std::unique_ptr<UniVariateNumbersGenerator> normalVariateGeneratorAntithetic = 
		std::make_unique<AntitheticWrapperUniVariateGenerator>(payoffObsSize, gaussianVariatesGenerator->clone());

	std::unique_ptr<StochasticPathGenerator> geomBrownMotionGenerator = std::make_unique<GBMPathGenerator>(
		payoffObservations,
		asianOption.m_underlying.GetReferencePrice(),
		r, vola,
		normalVariateGeneratorAntithetic->clone()
	);

	/*
	 * Statistics gatherers
	 */
	std::unique_ptr<CompositeStatisticsGatherer> compositeStatGatherer(new CompositeStatisticsGatherer());
	compositeStatGatherer
		->AddChildStatGatherer(std::make_unique<MomentsEvaluator>(2));
		// ->AddChildStatGatherer(std::make_unique<FullSampleGatherer>());

	/*
	 * Start multi-threading routine
	 */

	// TODO ignore rounding error (for a high number of simulations this is negligible)
	const unsigned long long N_SIMUL_PER_THREAD = NUM_SIMUL / NUM_THREAD;

	std::vector<std::unique_ptr<StatisticsGatherer>> statisticsGatherers;
	for (unsigned int i = 0; i < NUM_THREAD; ++i) {
		statisticsGatherers.push_back(compositeStatGatherer->clone());
	}

	std::vector<std::thread> thread_vec;
	int thread_seed = 1;
	for (const auto& statGatherer : statisticsGatherers) {
		std::unique_ptr<StochasticPathGenerator> pathGenerator = geomBrownMotionGenerator->clone();

		// TODO: find a cleaner and more reasoned way to set the seed in each thread
		pathGenerator->SetVariateGeneratorSeed(thread_seed++);

		MonteCarloEngine mcEngine{
			N_SIMUL_PER_THREAD,
			payoffObsSize,
			std::move(pathGenerator),
			payoffFunc,
			statGatherer.get()
		};
		thread_vec.emplace_back(std::move(mcEngine));
	}

	// join all threads
	std::for_each(thread_vec.begin(), thread_vec.end(), [](auto& thr){ thr.join(); });

	/*
	 * TODO the merging of info is implemented just for the MomentsEvaluator
	 * TODO find a more structured way to merge the results coming from each thread
	 * gather the results from each thread and merge them to get the complete statistical results
	 */
	std::vector<std::vector<double>> momentsPerThreadVec;
	int idx = 1;
	for (const auto& statGatherer : statisticsGatherers) {
		const auto& fullInfoTable = statGatherer->GetStatisticalInfo();

		auto momentsInfoTable_it = std::find_if(
			fullInfoTable.cbegin(), fullInfoTable.cend(),
			[](const auto& infoTable) {return (infoTable.first == MOMENTS_STRING);}
		);
		momentsPerThreadVec.push_back(std::move(momentsInfoTable_it->second));

		// Print info table for each thread in a separate file
		StatisticsGatherer::DownloadStatisticalInfoTable(fullInfoTable, "_thread" + std::to_string(idx++));
	}
	_StatisticalInfoTable mergedMomentsTable = _StatisticalInfoTable(1,	MomentsEvaluator::MergeMomentsInfo(momentsPerThreadVec));
	StatisticsGatherer::PrintStatisticalInfoTable(mergedMomentsTable);
	StatisticsGatherer::DownloadStatisticalInfoTable(mergedMomentsTable);

	const auto& moments = mergedMomentsTable[0].second;

	const double finalPrice = moments[0];
	const double M2 = moments[1];

	// sigma/sqrt(n) = sqrt(M2 - M1^2) / sqrt(n) (NB: finalPrice == M1)
	const double stdDevMean = std::sqrt((M2 - finalPrice * finalPrice) / (0.5 * NUM_SIMUL - 1));

	std::cout << "Final MonteCarlo price: " << finalPrice << "\n";
	std::cout << "Std dev of the mean: " << stdDevMean << "\n\n";
}
