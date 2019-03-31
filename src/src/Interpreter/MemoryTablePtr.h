#pragma once
#include <memory>

namespace ska {
	class MemoryTable;
	using MemoryTablePtr = std::shared_ptr<MemoryTable>;
	using MemoryTableCPtr = std::shared_ptr<const MemoryTable>;
}
