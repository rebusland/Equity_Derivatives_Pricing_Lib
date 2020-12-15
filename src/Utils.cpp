#include "Utils.h"

#include <cmath>
#include <numeric>

double Utils::ComputeAverage(const std::vector<double>& data, const AvgType& avgType) {
	unsigned int n = data.size();

	if (avgType == AvgType::GEOMETRIC) {
		auto logSumLambda = [](double init, double elem){return init + std::log(elem);};
		return std::exp(std::accumulate(data.cbegin(), data.cend(), 0.0, logSumLambda) / n);
	}

	// Simple Arithmetic Average
	return std::accumulate(data.cbegin(), data.cend(), 0.0) / n;
}

Utils::RollingAverage::RollingAverage(double value, const AvgType& avgType):
	m_value{value}, m_avg_type{avgType} {}

void Utils::RollingAverage::AddValue(double elem) {
	// increment counter
	++m_counter;

	if (m_avg_type == AvgType::GEOMETRIC) {
		m_value += std::log(elem);
		return;
	}

	// Simple Arithmetic Average
	m_value += elem;
}

double Utils::RollingAverage::AddValueAndGetAverage(double elem) {
	AddValue(elem);
	return GetAverage();
}

double Utils::RollingAverage::GetAverage() const {
	if (m_avg_type == AvgType::GEOMETRIC) {
		return std::exp(m_value / (double) m_counter);
	}

	// simple arithmetic average
	return m_value / (double) m_counter;
}

void Utils::RollingAverage::ResetAverage(double resetValue) {
	m_value = resetValue;
	m_counter = 0;
}
