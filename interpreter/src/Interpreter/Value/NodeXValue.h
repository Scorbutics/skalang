#pragma once
#include "Interpreter/MemoryTablePtr.h"
#include "NodeValue.h"

namespace ska {
	
	struct NodeLValue {
		NodeValue* object;
		MemoryTablePtr memory;
	};
	
	struct NodeRValue {
		NodeValue object;
		MemoryTablePtr memory;
	};
}