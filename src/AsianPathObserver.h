#ifndef _ASIAN_PATH_OBSERVER_H_
#define _ASIAN_PATH_OBSERVER_H_

#include <vector>

#include "PathObserver.h"
#include "Utils.h"

#include <iostream>

using _Date = int;

enum class AvgType;

class AsianPathObserver : public PathObserver {
	public:
		AsianPathObserver(
			const std::vector<_Date>& observations, 
			const AvgType& avgType
		);

		bool IsCompleted() const override {
			return m_it_observations == m_observation_dates.end();
		}

		void ObservePriceAtDate(const _Date& currentDate, double S) override;
		void ResetObserver() override;
		double GetAverage() const;

	private:
		/*const */std::vector<_Date>/*&*/ m_observation_dates; // TODO HOW TO CONVERT CONST ITERATOR TO SIMLE ITERATOR????!!!!
		std::vector<_Date>::iterator m_it_observations;
		Utils::RollingAverage m_rollingAvg;
};

#endif
