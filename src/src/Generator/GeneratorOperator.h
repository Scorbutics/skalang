#pragma once

#include "NodeValue/Operator.h"
#include "BytecodeGenerator.h"
#include "GeneratorOperatorUnit.h"
#include "Interpreter/Value/ExecutionContext.h"

#define SKALANG_GENERATOR_OPERATOR_DEFINE(OperatorType)\
    template<>\
    class GeneratorOperator<OperatorType> : \
		public GeneratorOperatorBase {\
	private:\
		using OperateOn = Operation<OperatorType>;\
	public:\
		using GeneratorOperatorBase::GeneratorOperatorBase;\
        BytecodeCell generate(ExecutionContext& node) override final {\
			return generate(OperateOn{node});\
		}\
	    BytecodeCell generate(OperateOn node);\
    };

namespace ska {
	class SymbolTable;
	class MemoryTable;
	class ASTNode;

	template <Operator O>
	class GeneratorOperator : 
		public GeneratorOperatorUnit {
	public:
		BytecodeCell generate(ExecutionContext node) override { return BytecodeCell {}; }
	};
}
