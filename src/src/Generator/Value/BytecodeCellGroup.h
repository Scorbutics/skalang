#pragma once
#include "NodeValue/Operator.h"
#include "NodeValue/Type.h"
#include "BytecodeValue.h"

namespace ska {
	using BytecodeCell = BytecodeRValue;
	using BytecodeCellGroup = std::vector<BytecodeCell>;
}
