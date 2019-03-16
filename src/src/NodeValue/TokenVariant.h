#pragma once
#include <variant>
#include <memory>

#include "Interpreter/ExecutionContext.h"

namespace ska {
	class ASTNode;
	class MemoryTable;
	using TokenVariant = std::variant<std::size_t, bool, int, double, std::string, std::shared_ptr<MemoryTable>, ExecutionContext>;
}