#ifndef _PROJECT_EXCEPTION_H_
#define _PROJECT_EXCEPTION_H_

#include <exception>
#include <sstream>
#include <string>

/**
 * Throws a custom ProjectException.
 * Use this macro to add information about the file name, the method and the line number where the exception occurred.
 */
#define THROW_PROJECT_EXCEPTION(message) \
	throw ProjectException(message, std::string(__FILE__), __FUNCTION__, __LINE__)

class ProjectException : public std::exception {
	public:
		/**
		 * Constructor.
		 * Creates a ProjectException with the specified message and source location.
		 * @param message the exception message.
		 * @param file the name of the source file where the exception occurred.
		 * @param method the name of the method where the exception occurred.
		 * @param line the line number in the source file where the exception occured.
		 * @throws no exception.
		 */
		explicit ProjectException(
			const std::string& message,
			const std::string& file = std::string(""),
			const std::string& method = std::string(""),
			unsigned long line = 0) throw()
		: mMessage(message) {
			// Create the trace message
			std::ostringstream ss;
			if (file.empty()) {
				ss << "\"" << message << "\"" << " (unknown source)";
			} else {
				// Find the last path separator and extract the base name of the
				// file in order to avoid the path to be in the trace message
				std::string::size_type pos = file.find_last_of("/\\");
				if (pos == std::string::npos) {
					ss << "\"" << message << "\"" << " (at " << method << ": " << file << ":" << line << ")";
				} else {
					std::string file_name_only = file.substr(pos + 1);
					ss << "\"" << message << "\"" << " (at " << method << ": " << file_name_only << ":" << line << ")";
				}
			}
			mErrorTrace = ss.str();
		}

		/**
		 * Destructor.
		 */
		virtual ~ProjectException() throw() {}

		/**
		 * Returns the exception message.
		 * @return the exception message.
		 * @throws no exception.
		 */
		virtual const char* what() const throw() {
			// return mMessage.c_str();
			return mErrorTrace.c_str();
		}

		/**
		 * Returns the trace message.
		 * The returned message contains the file name and the line number where
		 * the exception occurred. This will be useful when using log/trace systems.
		 * @return the trace message.
		 * @throws no exception.
		 */
		/* TODO for future use
		virtual const char* trace() const throw() {
			return mErrorTrace.c_str();
		}
		*/

	private:
		std::string mMessage;
		std::string mErrorTrace;
};

#endif
