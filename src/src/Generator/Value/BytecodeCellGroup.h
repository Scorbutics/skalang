#pragma once
#include <ostream>
#include "NodeValue/Operator.h"
#include "NodeValue/Type.h"
#include "BytecodeValue.h"

namespace ska {
	using BytecodeCell = BytecodeRValue;
	using BytecodeCellGroup = std::vector<BytecodeCell>;
	
	std::ostream& operator<<(std::ostream& stream, const BytecodeCellGroup&);
}
