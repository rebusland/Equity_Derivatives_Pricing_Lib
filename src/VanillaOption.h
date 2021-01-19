#ifndef _VANILLA_OPTION_H_
#define _VANILLA_OPTION_H_

#include "CallPut.h"
#include "Derivative.h"

class VanillaOption : public Derivative {
	public:
		VanillaOption(
			const Underlying& underlying,
			const _Date& issueDate,
			const _Date& expiryDate,
			const CallPut& callPut,
			double strike
		) : Derivative(underlying, issueDate, expiryDate), m_call_put{callPut}, m_strike{strike} {}

		const CallPut m_call_put;
		const double m_strike;
};

#endif
