#pragma once
#include <variant>
#include <memory>

#include "Interpreter/Value/ExecutionContext.h"
#include "BridgeMemory.h"
#include "ObjectMemory.h"

namespace ska {
	using TokenVariant = std::variant<std::size_t, bool, int, double, std::string, ExecutionContext, ObjectMemory, BridgeMemory>;
}
