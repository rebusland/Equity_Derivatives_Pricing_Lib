#include "AsianPathObserver.h"

#include "AvgType.h"

AsianPathObserver::AsianPathObserver(const std::vector<_Date>& observations, const AvgType& avgType) :
	m_observation_dates{observations},
	m_it_observations{m_observation_dates.begin()},
	m_rollingAvg{0.0, avgType} {}

void AsianPathObserver::ObservePriceAtDate(const _Date& currentDate, double S) {
	if (not IsCompleted() && currentDate == *m_it_observations) {
		m_rollingAvg.AddValue(S);
		std::advance(m_it_observations, 1);
	}
}

void AsianPathObserver::ResetObserver() {
	m_rollingAvg.ResetAverage(0.0);
	m_it_observations = m_observation_dates.begin();
}

double AsianPathObserver::GetAverage() const {
	if (not IsCompleted()) {
		// throw exception, trying to retrieve the result when the observer is still monitoring the MC path
		std::cerr << "Error in path observer: trying to retrieve result but the observer is still pending\n";
	}
	
	return m_rollingAvg.GetAverage();
}
