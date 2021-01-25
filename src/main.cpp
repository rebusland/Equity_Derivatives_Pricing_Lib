#include "PricerEndpointHandler.h"
#include "Timer.h"

/**
 * TODO:
 *  - Expose the endpoint to the input source of the data and the business logical
 *  - Interfacing with Excel (or Python, maybe Jupyter notebook?)
 *  - implement the Builder pattern for constructing complex objects!!
 *  - standardize project structures in packages/folders
 *  - Unit testing (Boost)
 *  - Try RNGs from Boost
 *  - Add results convergence analysis
 *  - Add logging framework (enable debugging code just when compiled in debug mode, perhaps via macros)
 */
int main() {
	_TIMER_MILLIS_(myTimer);

	PricerEndpointHandler::EvaluateBlack();
}
