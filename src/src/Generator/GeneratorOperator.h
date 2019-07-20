#pragma once

#include "NodeValue/Operator.h"
#include "BytecodeGenerator.h"
#include "GeneratorOperatorUnit.h"
#include "BytecodeGenerationContext.h"

#define SKALANG_GENERATOR_OPERATOR_DEFINE(OperatorType)\
    template<>\
    class GeneratorOperator<OperatorType> : \
		public GeneratorOperatorBase {\
	private:\
		using OperateOn = OperationType<OperatorType>;\
	public:\
		using GeneratorOperatorBase::GeneratorOperatorBase;\
        BytecodeCellGroup generate(BytecodeGenerationContext& node) override final {\
			return generate(OperateOn{node.pointer()}, node);\
		}\
		BytecodeCellGroup generate(OperateOn node, BytecodeGenerationContext& context);\
    };

namespace ska {
	template <Operator O>
	class GeneratorOperator;
}