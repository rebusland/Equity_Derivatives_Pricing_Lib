#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "CompositeStatisticsGatherer.h"
#include "Derivative.h"
#include "Discounter.h"
#include "FullSampleGatherer.h"
#include "GBMPathGenerator.h"
#include "JSONReader.h"
#include "MomentsEvaluator.h"
#include "MonteCarloEngine.h"
#include "MonteCarloSettings.h"
#include "PayoffFactory.h"
#include "Timer.h"
#include "UniVariateNumbersGenerator.h"
#include "UniVariateNumbersGeneratorFactory.h"

// TODO use unsigned long instead?
using _Date = long;

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

	const unsigned int payoffObsSize = payoffObservations.size();

	/*
	 * Random numbers generators and stochastic process generator
	 */
	auto variateNumbersGenerator = UniVariateNumbersGeneratorFactory::GetInstance().CreateVariateGenerator(
		generatorType,
		shouldApplyAntitheticWrapper,
		payoffObsSize,
		seed
	);

	std::unique_ptr<StochasticPathGenerator> geomBrownMotionGenerator = std::make_unique<GBMPathGenerator>(
		payoffObservations,
		(derivativePtr->m_underlying).GetReferencePrice(),
		r, vola,
		std::move(variateNumbersGenerator)
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
	const size_t N_SIMUL_PER_THREAD = NUM_SIMUL / NUM_THREAD;

	std::vector<std::unique_ptr<StatisticsGatherer>> statisticsGatherers;
	for (unsigned int i = 0; i < NUM_THREAD; ++i) {
		statisticsGatherers.push_back(compositeStatGatherer->clone());
	}

	std::vector<std::thread> thread_vec;
	int thread_seed = 1;
	for (const auto& statGatherer : statisticsGatherers) {
		std::unique_ptr<StochasticPathGenerator> pathGenerator = geomBrownMotionGenerator->Clone();

		// TODO: find a cleaner and more reasoned way to set the seed in each thread
		pathGenerator->SetVariateGeneratorSeed(thread_seed++);

		MonteCarloEngine mcEngine{
			N_SIMUL_PER_THREAD,
			payoffObsSize,
			std::move(pathGenerator),
			payoff->Clone(),
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
