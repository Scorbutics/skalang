#pragma once
#include <Logging/Logger.h>

#define SLOG(logger, level, currentClass) SKA_LOGC(logger, level, currentClass) << SKA_CURRENT_FUNCTION << "(" << __FILE__ << " " << __LINE__ << ") "