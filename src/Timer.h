#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <iostream>

#include "ProjectException.h"

/**
 * This instantiates a Timer object variable named "timerName": this variable name can then be used
 * throughout the code, since it gets expanded by the preprocessor before actual compilation.
 * The macro also adds information about the file name and the method in which the timer is used.
 */
#define _TIMER_(timerName, Duration) \
	Timer<Duration> timerName{std::string(__FILE__), __FUNCTION__}

#define _TIMER_SECONDS_(timerName) _TIMER_(timerName, seconds)
#define _TIMER_MILLIS_(timerName) _TIMER_(timerName, milliseconds)
#define _TIMER_NANOS_(timerName) _TIMER_(timerName, nanoseconds)

using namespace std::chrono;

template<class T_Duration = duration<class U, class R>>
class Timer {
	public:
		Timer(
			const std::string& file = std::string(""),
			const std::string& method = std::string("")
		) : m_file_name{file}, m_method_name{method} {
			constexpr bool isUnitSeconds = std::is_same<T_Duration, seconds>::value;
			constexpr bool isUnitMillis = std::is_same<T_Duration, milliseconds>::value;
			constexpr bool isUnitNanos = std::is_same<T_Duration, nanoseconds>::value;

			if (isUnitSeconds) {
				m_time_unit_string = "sec";
			} else if (isUnitMillis) {
				m_time_unit_string = "millis";
			} else if (isUnitNanos) {
				m_time_unit_string = "nanos";
			} else {
				THROW_PROJECT_EXCEPTION("Unsupported duration type");
			}

			// Start time is set at the very end of the constructor so as not to take into 
			// account, while timing, also the time spent to build the Timer object.
			m_start = steady_clock::now();
		}

		void Start() {
			m_is_stopped = false;
			m_start = steady_clock::now();
		}

		void Stop() {
			if (m_is_stopped) {
				THROW_PROJECT_EXCEPTION("Timer already stopped");
			}
			PrintElapsed();
			m_is_stopped = true;
		}

		~Timer() {
			if (not m_is_stopped) {
				PrintElapsed();
			}
		}

	private:
		void PrintElapsed() {
			std::cout << m_method_name << " Elapsed: " <<
			duration_cast<T_Duration>(steady_clock::now() - m_start).count() <<
			" " << m_time_unit_string << "\n";
		}

		time_point<std::chrono::steady_clock> m_start;
		bool m_is_stopped = false;

		// info about the code point where timer is used
		std::string m_file_name;
		std::string m_method_name;

		std::string m_time_unit_string;
};

#endif
