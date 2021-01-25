#include "MonteCarloHandler.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

#include "CompositeStatisticsGatherer.h"
#include "FullSampleGatherer.h"
#include "MomentsEvaluator.h"
#include "MonteCarloEngine.h"
#include "StatisticsGatherer.h"

// TODO IMPORTANT: we assume that the minimum time step is a single day.
// This is the time step used by default in the underlying SDE's equation, generating the MC paths.
constexpr double MIN_TIME_STEP = 1;

MonteCarloHandler::MonteCarloHandler(
	size_t nSimulations,
	unsigned int nThreads,
	std::unique_ptr<StochasticPathGenerator> pathGenerator,
	std::unique_ptr<Payoff> payoff
) : m_n_simulations{nSimulations}, m_n_threads{nThreads},
	m_stochastic_path_generator(std::move(pathGenerator)),
	m_payoff(std::move(payoff)) {}

void MonteCarloHandler::Run() const {
	/*
	 * Statistics gatherers
	 * TODO add settings to customize statistics gatherers and handle their construction
	 */
	std::unique_ptr<CompositeStatisticsGatherer> compositeStatGatherer(new CompositeStatisticsGatherer());
	compositeStatGatherer
		->AddChildStatGatherer(std::make_unique<MomentsEvaluator>(2));
		// ->AddChildStatGatherer(std::make_unique<FullSampleGatherer>());

	std::vector<std::unique_ptr<StatisticsGatherer>> statisticsGatherers;
	for (unsigned int i = 0; i < m_n_threads; ++i) {
		statisticsGatherers.push_back(compositeStatGatherer->clone());
	}

	/*
	 * Start multi-threading routine
	 */

	// TODO ignore rounding error (for a high number of simulations this is negligible)
	const size_t nSimulPerThread = m_n_simulations / m_n_threads;

	std::vector<std::thread> thread_vec;
	int thread_seed = 1;
	for (const auto& statGatherer : statisticsGatherers) {
		std::unique_ptr<StochasticPathGenerator> pathGenerator = m_stochastic_path_generator->Clone();

		// TODO: find a cleaner and more reasoned way to set the seed in each thread
		pathGenerator->SetVariateGeneratorSeed(thread_seed++);

		MonteCarloEngine mcEngine{
			nSimulPerThread,
			std::move(pathGenerator),
			m_payoff->Clone(),
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
	const double stdDevMean = std::sqrt((M2 - finalPrice * finalPrice) / (0.5 * m_n_simulations - 1));

	std::cout << "Final MonteCarlo price: " << finalPrice << "\n";
	std::cout << "Std dev of the mean: " << stdDevMean << "\n\n";
}
