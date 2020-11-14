#include <iostream>
#include <vector>

#include "AsianOption.h"
#include "Underlying.h"
#include "GeometricBrownianMotion.h"
#include "MonteCarloEngine.h"
#include "MonteCarloSettings.h"
#include "AsianPayoff.h"
#include "Utils.h"

using _Date = int;

/**
 * TODO:
 *  - implement the Builder pattern for constructing complex objects!!
 *
 */

int main() {

	Underlying underlying{"BASE", 10.0, 0.0};
	_Date startDate = 0;
	_Date expiryDate = 100;
	std::vector<_Date> strikeFixingDates = {2, 3, 5, 6};
	std::vector<_Date> priceFixingDates = {91, 92, 95, 96, 97, 98};
	CallPut callPut{CallPut::CALL};
	AvgType avgTypeStrike{AvgType::ARITHMETIC};
	AvgType avgTypePrice{AvgType::ARITHMETIC};

	AsianOption asianOption{
		underlying, startDate, expiryDate,
		callPut, strikeFixingDates, avgTypeStrike,
		priceFixingDates, avgTypePrice
	};

	double r = 0.00012; // this corresponds to an annual rate of return of 3% ca.
	double vola = 0.009; // this corresponds to annual volatility of 14% ca.
	GeometricBrownianMotion GBMSde{r, vola};

	const int N_MC_SIMULATIONS = 1000;
	
	MonteCarloSettings mcSettings{};
	AsianPayoff asianPayoff{asianOption};

	MonteCarloEngine mcEngine{
		&mcSettings,
		GBMSde,
		asianOption.m_start_date,
		asianOption.m_expiry_date,
		asianOption.m_underlying.m_ref_price,
		asianPayoff
	};

	Utils::RollingAverage rollingAvgMonteCarlo{0.0, AvgType::ARITHMETIC};

	// this section (for loop) will be moved inside MCEngine and calculation will take into
	// account the MC settings (i.e. multithreaded, antithetic variate etc.)
	double simulationPrice;
	for (int i = 0; i < N_MC_SIMULATIONS; i++) {
		// price estimeted in the current scenario
		simulationPrice = mcEngine.RunSimulation();
		std::cout << "Simulation " << i << " price: " << simulationPrice << "\n";
		rollingAvgMonteCarlo.AddValue(simulationPrice);
	}

	double finalPrice = rollingAvgMonteCarlo.GetAverage();
	std::cout << "Final MonteCarlo price: " << finalPrice << "\n";
}
