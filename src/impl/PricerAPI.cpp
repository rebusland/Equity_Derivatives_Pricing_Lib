#include "PricerAPI.h"

#include <string>

namespace pricer_api {

	/*
	 * A buffer for the results of the calls to the pricing endpoints.
	 * Being in the global scope and not on a specific function stack, this buffer is used to
	 * allow returning a const char pointer from the C APIs: char* cannot be returned directly from
	 * the APIs as the local char array pointed to gets removed from the stack as the function returns.
	 * IMPORTANT/TODO:
	 *  - a single string buffer is used for each pricer API; this should not pose problem as long as
	 *	  endpoints are called synchronously.
	 *	- each process loading this shared library will have its own "copy" of the data segment (i.e. library variables,
	 *	  and therefore the string resultBuffer): only the text (code) segment is actually shared among processes.
	 *  - check how and whether the present approach should be reviewed in a (eventually) multithreaded context.
	 */
	static std::string resultBuffer;

	const char* SHARED_API EvaluateBlack(
		const char* flatMktDataJSONString,
		const char* mcSettingsJSONString,
		const char* productJSONString
	) {
		resultBuffer = PricerEndpointHandler::EvaluateBlack(
			flatMktDataJSONString,
			mcSettingsJSONString,
			productJSONString
		);

		return resultBuffer.c_str();
	}

}
