#ifndef _PATH_OBSERVER_H_
#define _PATH_OBSERVER_H_

using _Date = int;

class PathObserver {
	public:
		virtual bool IsCompleted() const = 0;
		virtual void ObservePriceAtDate(const _Date& currentDate, double S) = 0;
		virtual void ResetObserver() = 0;
};

#endif
