#ifndef _PRICER_API_H_
#define _PRICER_API_H_

#include "SHARED_API.h"
#include "PricerEndpointHandler.h"

namespace pricer_api {

	/**
	 * C compliant APIs decoupling the outside-world calls to the shared library from the actual endpoints implementation.
	 */
	extern "C" {

		const char* SHARED_API EvaluateBlack(
			const char* flatMktDataJSONString,
			const char* mcSettingsJSONString,
			const char* productJSONString
		);

	}

}

#endif
