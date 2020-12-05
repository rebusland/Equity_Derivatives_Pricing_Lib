#ifndef _GEOMETRIC_BROWNIAN_MOTION_H_
#define _GEOMETRIC_BROWNIAN_MOTION_H_

#include <functional>

#include "Sde.h"

using generator_func = std::function<double ()>;

class GeometricBrownianMotion : public Sde {
	public:
		GeometricBrownianMotion(double r, double vola, const generator_func& gaussGenerator);
		double operator() (double S, double dt) override;

	private:
		generator_func m_gaussian_variate_generator;
};

#endif
