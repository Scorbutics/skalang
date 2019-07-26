#include <string>
#include "GeneratorOperatorVariable.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

namespace ska {
	namespace bytecode {
		static GenerationOutput CommonGenerate(Generator& generator, Value dest, const ska::ASTNode& node, GenerationContext& context) {
			auto contextName = context.pointer().name();
			//context.script().setGroup(contextName.empty() ? " " : std::move(contextName));
			auto valueGroup = generator.generate({ context.script(), node });
			valueGroup.push(InstructionPack{ Instruction { Command::MOV, dest , Value { node.name(), node.type().value() } } });
			//context.script().setGroup("");
			return valueGroup;
		}
	}
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_AFFECTATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(m_generator, { node.GetVariableNameNode().name(), node.GetVariableNameNode().type().value() }, node.GetVariableValueNode(), context);
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	return CommonGenerate(m_generator, { node.GetVariableName(), node.GetVariableValueNode().type().value() } , node.GetVariableValueNode(), context);
}
