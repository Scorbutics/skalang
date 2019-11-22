#pragma once
#include "Generator/Value/BytecodeInstruction.h"
#include "Generator/Value/BytecodeInstructionOutput.h"
#include "NodeValue/LogicalOperator.h"
#include "NodeValue/ExpressionType.h"

namespace ska {
	struct Type;
	namespace bytecode {
		class Script;
		struct TypedValueRef {
			TypedValueRef(const Type& type, const Value value) : type(type), value(value) {}
			const Type& type;
			const Value value;
		};

		InstructionOutput TypeConversionBinary(LogicalOperator logicalOperator, const TypedValueRef& node1, const TypedValueRef& node2, const TypedValueRef& destination);
	}
}
