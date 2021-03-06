#ifndef _FULL_SAMPLE_GATHERER_H_
#define _FULL_SAMPLE_GATHERER_H_ 

#include "StatisticsGatherer.h"

#include <vector>

#define FULL_DATA_STRING "Full data"

/**
 * Gather all the samples, basically it's a decorator to the vector of samples exposing 
 * a StatisticsGatherer's interface. All the results gathered can then be dumped to an external
 * system (e.g. Excel) in which tailored statistical analysis can be performed; results can also
 * be used to compute statistical quantities which depends on the full sample and cannot be updated
 * in a rolling fashion, such as quartiles (although even for these approximate rolling algo exists).
 */
class FullSampleGatherer : public StatisticsGatherer {
	public:
		void AcquireResult(double simulationResult) override {
			m_samples.push_back(simulationResult);
		}

		_StatisticalInfoTable GetStatisticalInfo() const override;

		std::unique_ptr<StatisticsGatherer> clone() const override;

	protected:
		std::vector<double> m_samples;
};

#endif
