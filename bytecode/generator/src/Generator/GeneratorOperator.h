#pragma once

#include "NodeValue/Operator.h"
#include "GeneratorOperatorUnit.h"
#include "BytecodeGenerationContext.h"

namespace ska {
	namespace bytecode {
		template <ska::Operator O>
		class GeneratorOperator;
	}
}

#define SKALANG_GENERATOR_OPERATOR_DEFINE(OperatorType)\
	template<>\
	class GeneratorOperator<OperatorType> :\
		public GeneratorOperatorBase {\
	private:\
		using OperateOn = OperationType<OperatorType>;\
	public:\
		using GeneratorOperatorBase::GeneratorOperatorBase;\
		ScriptGenerationOutput generate(GenerationContext& node) override final {\
			return generate(OperateOn{node.pointer()}, node);\
		}\
		ScriptGenerationOutput generate(OperateOn node, GenerationContext& context);\
	};
