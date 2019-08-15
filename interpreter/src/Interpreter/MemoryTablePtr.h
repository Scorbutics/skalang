#pragma once
#include <memory>

namespace ska {
	class NodeValue;
	class MemoryTable;
	using MemoryTablePtr = std::shared_ptr<MemoryTable>;
	using MemoryTableCPtr = std::shared_ptr<const MemoryTable>;
	using MemoryLValue = std::pair<NodeValue*, MemoryTablePtr>;
	using MemoryCLValue = std::pair<const NodeValue*, MemoryTableCPtr>;
}
