#ifndef _DISCOUNTER_H_
#define _DISCOUNTER_H_

#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

// for placeholders 
using namespace std::placeholders; 

#ifndef _DATE_DEF_
#define _DATE_DEF_
using _Date = long;
#endif

/**
 * The first prototype of a discounter class.
 */
class Discounter {
	public:
		// This class cannot be instantiated
		Discounter() = delete;

		/**
		 * Returns the discount factor from end to start date
		 * The flat interest rate should be replaced by the proper discount curve.
		 * TODO Ok only if rate and dates are expressed in the same units (e.g. days)
		 */
		static double Discount(const _Date& startDate, const _Date& endDate, double r) {
			return std::exp(-r * (endDate - startDate));
		}

		/**
		 * An API to get multiple discount factors at once (rely on NRVO)
		 */
		static std::vector<double> Discount(
			const _Date& startDate,
			const std::vector<_Date>& paymentDates, 
			double r
		) {
			std::vector<double> discounts(paymentDates.size());
			const auto f_single_discount = [&](_Date paymentDate){ return Discount(startDate, paymentDate, r); };

			std::transform(paymentDates.begin(), paymentDates.end(), discounts.begin(), f_single_discount);
			return discounts;
		}
};

#endif
