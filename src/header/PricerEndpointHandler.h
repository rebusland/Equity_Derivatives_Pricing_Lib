#ifndef _PRICER_ENDPOINT_HANDLER_H
#define _PRICER_ENDPOINT_HANDLER_H

#include <string>

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
class PricerEndpointHandler {
	public:
		static std::string EvaluateBlack(
			const std::string& flatMktDataJSONString,
			const std::string& mcSettingsJSONString,
			const std::string& productJSONString
		);

	private:
		PricerEndpointHandler() {}
};

#endif
