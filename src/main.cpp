#include <iostream>
#include <functional>
#include <random>
#include <vector>

#include "AsianOption.h"
#include "AsianPayoff.h"
#include "CompositeStatisticsGatherer.h"
#include "Discounter.h"
#include "FullSampleGatherer.h"
#include "GeometricBrownianMotion.h"
#include "MomentsEvaluator.h"
#include "MonteCarloEngine.h"
#include "MonteCarloSettings.h"
#include "StatePayoff.h"
#include "Underlying.h"

using _Date = int;

// TODO IMPORTANT: we assume that the minimum time step is a single day.
// This is the time step used by default in the underlying SDE's equation, generating the MC paths.
constexpr double MIN_TIME_STEP = 1;

/**
 * TODO:
 *  - implement the Builder pattern for constructing complex objects!!
 *  - standardize project structures in packages/folders
 *  - add more modularity to the code! Start splitting this main
 *  - standardize and check memory management accross code entities, namely use smart pointers as much as possible.
 */

int main() {

	Underlying underlying{"BASE", 10.0, 0.0};
	_Date startDate = 0;
	_Date optionIssueDate = 0; // we assume the evaluationDate equals the issue date of the option
	_Date expiryDate = 251;
	std::vector<_Date> strikeFixingDates = {0}; // {2, 3, 5, 6};
	std::vector<_Date> priceFixingDates = {251}; // {91, 92, 95, 96, 97, 98};
	CallPut callPut{CallPut::CALL};
	AvgType avgTypeStrike{AvgType::ARITHMETIC};
	AvgType avgTypePrice{AvgType::ARITHMETIC};

	AsianOption asianOption{
		underlying, optionIssueDate, expiryDate,
		callPut, strikeFixingDates, avgTypeStrike,
		priceFixingDates, avgTypePrice
	};

	double r = 0.00012; // this corresponds to an annual rate of return of 3% ca.
	double vola = 0.009; // this corresponds to annual volatility of 14% ca.

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
	auto normalVariateGenerator = std::bind(normalDistr, uniformRng);

	GeometricBrownianMotion GBMSde{r, vola, MIN_TIME_STEP, normalVariateGenerator};

	MonteCarloSettings mcSettings{
		SimulationScheduler::SEQUENTIAL,
		VarianceReduction::NONE,
		10000 // number of simulations
	};

	const double discountFactor = Discounter::Discount(startDate, asianOption.m_expiry_date, r);
	AsianPayoff asianPayoff{asianOption, discountFactor};

	MomentsEvaluator momentsEvaluator{4};
	FullSampleGatherer fullSampleGatherer;
	CompositeStatisticsGatherer compositeStatGatherer{
		&momentsEvaluator, // TODO use smart pointers instead?
		&fullSampleGatherer
	};

	MonteCarloEngine mcEngine{
		mcSettings,
		GBMSde,
		startDate,
		asianOption.m_expiry_date,
		asianOption.m_underlying.GetReferencePrice(),
		&asianPayoff, // TODO use smart pointers instead?
		compositeStatGatherer
	};

	double finalPrice = mcEngine.EvaluatePayoff();
	std::cout << "Final MonteCarlo price: " << finalPrice << std::endl;

	StatisticsGatherer::PrintStatisticalInfoTable(compositeStatGatherer.GetStatisticalInfo());
}
