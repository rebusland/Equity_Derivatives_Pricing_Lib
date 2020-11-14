#ifndef _GEOMETRIC_BROWNIAN_MOTION_H_
#define _GEOMETRIC_BROWNIAN_MOTION_H_

#include <random>

#include "Sde.h"

class GeometricBrownianMotion : public Sde {
	public:
		GeometricBrownianMotion(double r, double vola);
		double operator() (double S, double dt) override;

	private:
		// TODO move those in a separate factory Builder for distributions and generators?
		std::default_random_engine m_generator; // TODO what seed should I use? Now is defaulted
		std::normal_distribution<double> m_norm_distribution; // by default on creation is N(0, 1) as needed
};

#endif
