#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include "NonCopyable.h"

template<class T>
class Singleton : private NonCopyable {
	public:
		// the local static variable is instantiated only once
		static T& GetInstance() {
			static T one;
			return one;
		}

	protected:
		Singleton() {}
};

#endif
