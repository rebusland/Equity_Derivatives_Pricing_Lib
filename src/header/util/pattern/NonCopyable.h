#ifndef _NON_COPYABLE_H_
#define _NON_COPYABLE_H_

/**
 * An interface to mark classes as not-copiable.
 * This pattern is described well in "Joshi - C++ and design patterns":
 * << The key observation that makes this technique work is that if a class has a
 * private copy constructor or assignment operator, then any class inherited from
 * it cannot be copied or assigned either, since the inherited class implicitly holds a
 * base class object.>>. Later is written, regarding space overhead: <<Generally, an object 
 * contains an instance of the base class so inheriting off something simply adds to the size of the base class.
 * However, this is not true when the base class is empty. In that case, the standard allows the compiler to 
 * optimize away the dummy space. So the private inheritance model has no overhead and the virtue of clarity.>>
 */
class NonCopyable {
	protected:
		NonCopyable() {}
		~NonCopyable() {}

		NonCopyable( const NonCopyable& ) = delete;
		const NonCopyable& operator=( const NonCopyable& ) = delete;
};

#endif
