#ifndef _PRICER_ENDPOINT_HANDLER_H
#define _PRICER_ENDPOINT_HANDLER_H

#include <string>

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
