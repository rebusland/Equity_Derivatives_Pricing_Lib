#include "GeometricBrownianMotion.h"
#include <cmath>

GeometricBrownianMotion::GeometricBrownianMotion(
		double r,
		double vola,
		double dt,
		const generator_func& gaussianGen
	) :	Sde{r, vola, dt},
	m_risk_free_drift{m_r * m_dt},
	m_vola_width{m_vola * std::sqrt(dt)},
	m_gaussian_variate_generator{gaussianGen} {}

double GeometricBrownianMotion::operator() (double S) {
	// Euler discretization scheme
	return S * (m_risk_free_drift + m_vola_width * m_gaussian_variate_generator());
}
