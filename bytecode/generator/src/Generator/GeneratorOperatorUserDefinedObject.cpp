#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorUserDefinedObject.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>::generate(OperateOn node, GenerationContext& context) {
	auto objectResult = GenerationOutput{ };

	for(auto& field: node) {
		auto valueGroup = generateNext({ context.script(), *field, context.scope() + 1 });
		valueGroup.push(Instruction { Command::PUSH, valueGroup.value() });
		objectResult.push(std::move(valueGroup));
	}

	return objectResult;
}
