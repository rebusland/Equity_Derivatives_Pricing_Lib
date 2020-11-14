#ifndef _ASIAN_OPTION_H_
#define _ASIAN_OPTION_H_

#include <vector>

#include "AvgType.h"
#include "CallPut.h"
#include "Derivative.h"

using _Date = int;

class AsianOption : public Derivative {
	public:
		AsianOption(
			const Underlying& underlying,
			const _Date& startDate,
			const _Date& expiryDate,
			const CallPut& callPut,
			const std::vector<_Date>& strikeFixingDates,
			const AvgType& strikeAvgType,
			const std::vector<_Date> priceFixingDates,
			const AvgType& priceAvgType
		) : Derivative(underlying, startDate, expiryDate), m_call_put{callPut},
		m_strike_fixing_dates{strikeFixingDates}, m_avg_type_strike{strikeAvgType},
		m_price_fixing_dates{priceFixingDates}, m_avg_type_price{priceAvgType} {}

	// private: ?
		CallPut m_call_put;

		// avg strike...
		std::vector<_Date> m_strike_fixing_dates;
		AvgType m_avg_type_strike;

		// ..oppure fixed strike
		// double m_strike;

		std::vector<_Date> m_price_fixing_dates;
		AvgType m_avg_type_price;
};

#endif
