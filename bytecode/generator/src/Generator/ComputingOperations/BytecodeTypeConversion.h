#pragma once
#include "Generator/Value/BytecodeInstruction.h"
#include "Generator/Value/BytecodeInstructionOutput.h"
#include "NodeValue/LogicalOperator.h"
#include "NodeValue/ExpressionType.h"

namespace ska {
	struct Type;
	namespace bytecode {
		class Script;
		struct TypedOperandRef {
			TypedOperandRef(const Type& type, const Operand operand) : type(type), operand(operand) {}
			const Type& type;
			const Operand operand;
		};

		InstructionOutput TypeConversionBinary(LogicalOperator logicalOperator, const TypedOperandRef& node1, const TypedOperandRef& node2, const TypedOperandRef& destination);
	}
}
