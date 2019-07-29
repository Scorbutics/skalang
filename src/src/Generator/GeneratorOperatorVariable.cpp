#include <string>
#include "GeneratorOperatorVariable.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

namespace ska {
	namespace bytecode {
		static GenerationOutput CommonGenerate(Generator& generator, const ASTNode& dest, const ska::ASTNode& node, GenerationContext& context) {
			auto valueGroup = generator.generate({ context.script(), node });
			if(dest.symbol() != node.symbol() || node.symbol() == nullptr) {
				valueGroup.push(InstructionPack{ Instruction { Command::MOV, context.script().queryVariableOrValue(dest) , context.script().queryVariableOrValue(node) } });
			}
			return valueGroup;
		}
	}
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_AFFECTATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(m_generator, node.GetVariableNameNode(), node.GetVariableValueNode(), context);
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(m_generator, node.GetVariableNameNode(), node.GetVariableValueNode(), context);
}
