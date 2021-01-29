#ifndef _UTILS_H_
#define _UTILS_H_

#include <memory>
#include <string>
#include <vector>

#include "AvgType.h"
#include "derivative/CallPut.h"

/**
 *	TODO:
 *  - Implement rolling/updating averages as well, in order to save memory during MC path trasversal (no need to save all values in a vector to be processed later).
 */

// fwd declarations
class PricingResults;
class PricingResultsGatherer;

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
			unsigned int m_counter = 0; // counts the number of elements which are averaged
			AvgType m_avg_type;
	};

	/**
	 * Sample standard deviation of the mean from the first moments
	 */
	double GetStdDevOfMeanFromMoments(double m1, double m2, size_t N);

	// TODO move mapping utilities in a separate environment/namespace (?)
	inline int FromCallPutEnumToInt(const CallPut& callPut) {
		return callPut == CallPut::CALL ? 1 : -1;
	}

	PricingResults ExtractPricingResultsFromGatherers(
		const std::vector<std::unique_ptr<PricingResultsGatherer>>& resultsGatherersPerThread,
		unsigned int nThreads,
		size_t nMoments,
		size_t nMCSimulations
	);

	std::string ReadJsonFileAsString(const char* relativePathFileName);
}

#endif
