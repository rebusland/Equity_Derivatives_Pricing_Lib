#include "GeometricBrownianMotion.h"

#include <cmath>

GeometricBrownianMotion::GeometricBrownianMotion(double r, double vola, const generator_func& gaussianGen) :
	Sde{r, vola}, m_gaussian_variate_generator{gaussianGen} {}

double GeometricBrownianMotion::operator() (double S, double dt) {
	return S * (m_r * dt + m_vola * std::sqrt(dt) * m_gaussian_variate_generator());
}
