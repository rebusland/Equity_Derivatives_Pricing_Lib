#ifndef _CLONABLE_H_
#define _CLONABLE_H_

#include <memory>

/**
 * An interface to emphasize that the derived class should provide a cloning mechanism for its instances. 
 */
template<class T>
class Clonable {
	public:
		virtual std::unique_ptr<T> Clone() const = 0;
};

#endif
