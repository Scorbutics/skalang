#pragma once
#include <variant>
#include <memory>

#include "BridgeMemory.h"
#include "NodeValue/ObjectMemory.h"
#include "NodeValue/StringShared.h"
#include "Generator/Value/BytecodeValue.h"

namespace ska {
	namespace bytecode {
		using TokenVariant = std::variant<std::size_t, bool, long, double, StringShared, ObjectMemory, BridgeMemory>;
	}
}
