#include "payoff/PayoffFactory.h"

std::unique_ptr<Payoff> PayoffFactory::CreatePayoff(
		std::string payoffId,
		Derivative* derivativeParams,
		const std::vector<double>& discounts
	) {
	const auto it = m_payoffId_to_builder_map.find(payoffId);
	if (it == m_payoffId_to_builder_map.end()) {
		THROW_PROJECT_EXCEPTION("Unsupported payoffId: " + payoffId);
	}
	return (it->second)(derivativeParams, discounts, payoffId);
}

void PayoffFactory::Register(std::string payoffId, PayoffBuilderFunc builderFunc) {
	m_payoffId_to_builder_map.insert({payoffId, builderFunc});
}
