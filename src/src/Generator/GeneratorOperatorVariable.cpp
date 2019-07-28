#include <string>
#include "GeneratorOperatorVariable.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

namespace ska {
	namespace bytecode {
		static GenerationOutput CommonGenerate(Generator& generator, const ASTNode& dest, const ska::ASTNode& node, GenerationContext& context) {
			auto contextName = context.pointer().name();
			//context.script().setGroup(contextName.empty() ? " " : std::move(contextName));
			auto valueGroup = generator.generate({ context.script(), node });
			valueGroup.push(InstructionPack{ Instruction { Command::MOV, context.script().queryVariableOrValue(dest) , context.script().queryVariableOrValue(node) } });
			//context.script().setGroup("");
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
