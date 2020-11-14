#ifndef _UTILS_H_
#define _UTILS_H_

#include <vector>

#include "AvgType.h"

/**
 *	TODO:
 *  - Implement rolling/updating averages as well, in order to save memory during MC path trasversal (no need to save all values in a vector to be processed later).
 */
namespace Utils {

	double ComputeAverage(const std::vector<double>& data, const AvgType& avgType);

	// TODO it can be generalized to RollingEvaluator (for example to check min, max etc.. based on a provided "fixing rule")
	class RollingAverage {
		public:
			RollingAverage(double value, const AvgType& avgType);
			void AddValue(double elem);
			double AddValueAndGetAverage(double elem);
			double GetAverage() const;
			void ResetAverage(double resetValue);

		private:
			double m_value;
			int m_counter = 0; // counts the number of elements which are averaged
			const AvgType& m_avg_type;
	};
}

#endif
