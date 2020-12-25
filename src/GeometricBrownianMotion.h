#ifndef _GEOMETRIC_BROWNIAN_MOTION_H_
#define _GEOMETRIC_BROWNIAN_MOTION_H_

#include <functional>

#include "Sde.h"

#include <cmath>

#include "UniVariateNumbersGenerator.h"

// using generator_func = std::function<double ()>;

class GeometricBrownianMotion : public Sde {
	public:
		GeometricBrownianMotion(
				double r,
				double vola,
				double dt,
				UniVariateNumbersGenerator* gaussianGen
			) :	Sde{r, vola, dt},
			m_risk_free_drift{m_r * m_dt},
			m_vola_width{m_vola * std::sqrt(dt)},
			m_gaussian_variate_generator{gaussianGen} {}

		double operator() (double S) override {
			// Euler discretization scheme
			// return S * (m_risk_free_drift + m_vola_width * m_gaussian_variate_generator());

			// TODO temporary: just for the purpose of testing the new variate generator classes
			return S * (m_risk_free_drift + m_vola_width * m_gaussian_variate_generator->GenerateSequence()[0]);
		}

	private:
		// NB precompute as much as possible!
		const double m_risk_free_drift;
		const double m_vola_width;
		// generator_func m_gaussian_variate_generator;
		UniVariateNumbersGenerator* m_gaussian_variate_generator;
};

#endif
