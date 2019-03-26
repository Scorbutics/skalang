#pragma once
#include <variant>
#include <memory>

#include "Interpreter/ExecutionContext.h"
#include "BridgeMemory.h"
#include "ObjectMemory.h"

namespace ska {
	class ASTNode;
	class MemoryTable;
	using TokenVariant = std::variant<std::size_t, bool, int, double, std::string, ExecutionContext, ObjectMemory, BridgeMemory>;
}
