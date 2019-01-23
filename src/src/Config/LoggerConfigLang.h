#pragma once
#include <Logging/MultiLogger.h>
#include <Logging/LogAsync.h>
#include <Logging/Logger.h>

namespace ska {
	namespace detail {
		using SkaLangLogger = ska::MultiLogger<
			ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogAsync>,
			ska::Logger<ska::LogLevel::Debug, ska::LogLevel::Error, ska::LogSync>
		>;
		SkaLangLogger BuildLogger(const char* filename);

		extern SkaLangLogger Logger;
	}
}
#ifndef SLOG_STATIC
#define SLOG_STATIC(level, currentClass) SKA_LOGC_STATIC(detail::Logger, level, currentClass)
#define SLOG(level) SKA_LOGC(detail::Logger, level)
#endif
