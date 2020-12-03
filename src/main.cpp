#include <iostream>
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
	_Date expiryDate = 100;
	std::vector<_Date> strikeFixingDates = {2, 3, 5, 6};
	std::vector<_Date> priceFixingDates = {91, 92, 95, 96, 97, 98};
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
	GeometricBrownianMotion GBMSde{r, vola};

	MonteCarloSettings mcSettings{
		SimulationScheduler::SEQUENTIAL,
		VarianceReduction::NONE,
		1000 // number of simulations
	};

	const double discountFactor = Discounter::Discount(asianOption.m_issue_date, asianOption.m_expiry_date, r);
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
