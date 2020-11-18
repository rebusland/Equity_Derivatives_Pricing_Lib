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
			const _Date& issueDate,
			const _Date& expiryDate,
			const CallPut& callPut,
			const std::vector<_Date>& strikeFixingDates,
			const AvgType& strikeAvgType,
			const std::vector<_Date> priceFixingDates,
			const AvgType& priceAvgType
		) : Derivative(underlying, issueDate, expiryDate), m_call_put{callPut},
		m_strike_fixing_dates{strikeFixingDates}, m_avg_type_strike{strikeAvgType},
		m_price_fixing_dates{priceFixingDates}, m_avg_type_price{priceAvgType} {}

	// TODO: I think the vectors are still not completely immutable
	const std::vector<_Date>& GetStrikeFixingDates() const {
		return m_strike_fixing_dates;
	}

	const std::vector<_Date>& GetPriceFixingDates() const {
		return m_price_fixing_dates;
	}

	const CallPut m_call_put;
	const AvgType m_avg_type_strike;
	// ..oppure fixed strike
	// double m_strike;
	const AvgType m_avg_type_price;

	private:
		std::vector<_Date> m_strike_fixing_dates;
		std::vector<_Date> m_price_fixing_dates;
};

#endif
