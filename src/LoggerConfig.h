#pragma once
#include <Logging/MultiLogger.h>
#include <Logging/LogAsync.h>
#include <Logging/Logger.h>

namespace detail {
	using SkaLangLogger = ska::MultiLogger<
		ska::Logger<ska::LogLevel::Info, ska::LogLevel::Error, ska::LogAsync>,
		ska::Logger<ska::LogLevel::Error, ska::LogLevel::Error, ska::LogSync>
	>;
	SkaLangLogger BuildLogger(const char* filename);

	extern SkaLangLogger Logger;
}

#define SLOG_STATIC(level, currentClass) detail::Logger.log<level, currentClass, __LINE__>(SKA_CURRENT_FUNCTION, __FILE__ )
#define SLOG(level) SLOG_STATIC(level, std::remove_reference<decltype(*this)>::type)