#pragma once
#include <variant>
#include <memory>

#include "ExecutionContext.h"
#include "BridgeMemory.h"
#include "NodeValue/ObjectMemory.h"
#include "NodeValue/StringShared.h"

namespace ska {
	using TokenVariant = std::variant<std::size_t, bool, int, double, StringShared, ExecutionContext, ObjectMemory, BridgeMemory>;
}
