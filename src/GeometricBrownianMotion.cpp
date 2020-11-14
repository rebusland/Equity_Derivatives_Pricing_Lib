#include "GeometricBrownianMotion.h"

GeometricBrownianMotion::GeometricBrownianMotion(double r, double vola) : Sde{r, vola} {}

double GeometricBrownianMotion::operator() (double S, double dt) {
	return S * (m_r * dt + m_vola * std::sqrt(dt) * m_norm_distribution(m_generator));
}
