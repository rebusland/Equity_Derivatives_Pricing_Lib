#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <iostream>

/**
 * Returns a Timer object.
 * The macro adds information about the file name and the method where the timer is used
 */
 // TODO How to handle template arguments??
#define _TIMER_() \
	Timer(std::string(__FILE__), __FUNCTION__)

using namespace std::chrono;

class Timer {
	public:
		Timer(
			const std::string& file = std::string(""),
			const std::string& method = std::string("")
		) : m_start{steady_clock::now()}, m_file_name{file}, m_method_name{method} {}

		// TODO method currently not used. It could be used if we want to reuse the same timer in the same scope.
		void Start() {
			m_is_stopped = false;
		}

		// TODO method currently not used. It could be used if we want to reuse the same timer in the same scope.
		void Stop() {
			m_is_stopped = true;
		}

		~Timer() {
			std::cout << m_method_name << " Elapsed: " << duration_cast<milliseconds>(steady_clock::now() - m_start).count() << " ms \n";
		}

	private:
		time_point<std::chrono::steady_clock> m_start;
		bool m_is_stopped = false;

		// info about the code point where timer is used
		std::string m_file_name;
		std::string m_method_name;
};

#endif
