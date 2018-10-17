#pragma once
#include <Logging/Logger.h>
#include <Logging/ColorStream.h>

#define SLOG(logger, level, currentClass) SKA_LOGC(logger, level, currentClass) << SKA_CURRENT_FUNCTION << ska::EnumColorStream::BROWN << " (" << __FILE__ << " " << __LINE__ << ") " << ska::EnumColorStream::WHITE