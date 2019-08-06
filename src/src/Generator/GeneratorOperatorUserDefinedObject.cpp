#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorUserDefinedObject.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>::generate(OperateOn node, GenerationContext& context) {
	auto objectResult = GenerationOutput{ InstructionPack{} };

	for(auto& field: node) {
		auto valueGroup = m_generator.generate({ context.script(), *field });
		valueGroup.push(Instruction { Command::PUSH, valueGroup.value() });
		objectResult.push(std::move(valueGroup));
	}

	return objectResult;
}
