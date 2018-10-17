#pragma once
#include <Logging/Logger.h>

namespace detail {
	
	template <class ... Logger>
	class LogCapturer {
	public:	
		template <ska::LogLevel level, class CurrentClass>
		auto log(const char* functionName, const char* filename, const char* line) {
			
		}
		
	private:
		template <ska::LogLevel level, class CurrentClass>
		auto logForLogger(auto& logger, const char* functionName, 
	
		std::tuple<Logger...> m_loggers;
	};
}

#define SLOG(level, currentClass) detail::LogCapturer::log<level, currentClass>(SKA_CURRENT_FUNCTION, __FILE__ ,__LINE__)