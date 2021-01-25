#include <string>

#include "PricerEndpointHandler.h"
#include "Timer.h"

#include "Utils.h"

/**
 * TODO:
 *  - Expose the endpoints to the input source of the data and the business logic
 *  - Interfacing with Excel (or Python, maybe Jupyter notebook?)
 *  - Implement the Builder pattern for constructing complex objects!!
 *  - Standardize project structure in packages/folders
 *  - Unit testing (Boost)
 *  - Try RNGs from Boost
 *  - Add results convergence analysis
 *  - Add logging framework (enable debugging code just when compiled in debug mode, perhaps via macros)
 */
int main() {
	_TIMER_MILLIS_(myTimer);

	/*
	 * TODO
	 * These should come from a caller, such as a GUI or a test suite.
	 */
	std::string flatMktDataString = Utils::ReadJsonFileAsString("input/flatMarketData.json");
	std::string mcSettingsString = Utils::ReadJsonFileAsString("input/monteCarloSettings.json");
	std::string productString = Utils::ReadJsonFileAsString("input/product.json");

	PricerEndpointHandler::EvaluateBlack(
		flatMktDataString,
		mcSettingsString,
		productString
	);
}
