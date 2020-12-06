#ifndef _GEOMETRIC_BROWNIAN_MOTION_H_
#define _GEOMETRIC_BROWNIAN_MOTION_H_

#include <functional>

#include "Sde.h"

using generator_func = std::function<double ()>;

class GeometricBrownianMotion : public Sde {
	public:
		GeometricBrownianMotion(double r, double vola, double dt, const generator_func& gaussGenerator);
		double operator() (double S) override;

	private:
		// NB precompute as much as possible!
		double m_risk_free_drift;
		double m_vola_width;
		generator_func m_gaussian_variate_generator;
};

#endif
