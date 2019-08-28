#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorUserDefinedObject.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>::generate(OperateOn node, GenerationContext& context) {
	auto objectResult = GenerationOutput{ };

	auto fields = std::vector<Value>{};

	for(auto& field: node) {
		auto valueGroup = generateNext({ context.script(), *field, context.scope() + 1 });
		fields.push_back(valueGroup.value());
		valueGroup.push(Instruction { Command::PUSH, valueGroup.value() });
		objectResult.push(std::move(valueGroup));
	}

	//context.script().storeFieldsUserDefined(std::move(fields));

	return objectResult;
}
