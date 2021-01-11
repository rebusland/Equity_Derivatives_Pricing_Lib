#include "StatisticsGatherer.h"

#include <vector>

#include "Utils.h"

using namespace Utils;

#define MOMENTS_STRING "Moments"

/**
 * A rolling evaluator of the first n moments, i.e. <X>, <X^2>, <X^3>...<X^n>, when n 
 * can be specified at runtime.
 * As they are computed in a rolling way they are not "standardized", in particular they are not
 * calculated based on the variable centered around the mean, e.g. <X -<X>>^2.
 * Thus, appropriate conversion functions must be applied to get relevant quantities such as:
 * - variance: <X - M1>^2 => M2 - M1^2
 * - skewness: <X - M1>^3 => M3 - 3*M1*M2 + 2*M1^3
 * - kurtosis: <X - M1>^4 => M4 - 4*M1*M3 +6*M1^2*M2 - 3*M1^4
 * where M1, M2 etc. are the raw moments computed by the present routine
 */
class MomentsEvaluator : public StatisticsGatherer {
	public:
		MomentsEvaluator(size_t);
		void AcquireResult(double) override;
		_StatisticalInfoTable GetStatisticalInfo() const override;
		std::unique_ptr<StatisticsGatherer> clone() const override;
		std::vector<double> GetMomentsSoFar() const;

		static _StatisticalInfo MergeMomentsInfo(const std::vector<std::vector<double>>& momentsInfoVec);

	private:
		std::vector<RollingAverage> m_rolling_moments;
};