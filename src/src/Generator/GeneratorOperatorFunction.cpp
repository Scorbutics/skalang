#include <string>
#include "GeneratorOperatorFunction.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

namespace ska {
	namespace bytecode {
		static GenerationOutput CommonGenerate(Generator& generator, const ASTNode& dest, const ska::ASTNode& node, GenerationContext& context) {
			auto valueGroup = generator.generate({ context.script(), node });
			valueGroup.push(InstructionPack{ Instruction { Command::MOV, context.script().queryVariableOrValue(dest) , context.script().queryVariableOrValue(node) } });			
			return valueGroup;
		}
	}
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_DECLARATION>::generate(OperateOn node, GenerationContext& context) {
	return context.script().queryVariableOrValue(node.GetFunction());
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FUNCTION_CALL>::generate(OperateOn node, GenerationContext& context) {
	return Value{};//CommonGenerate(m_generator, node.GetVariableNameNode(), node.GetVariableValueNode(), context);
}
