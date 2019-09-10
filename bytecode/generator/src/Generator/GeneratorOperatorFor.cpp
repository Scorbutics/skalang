#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorFor.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FOR_LOOP>::generate(OperateOn node, GenerationContext& context) {

	auto initGroup = generateNext({ context.script(), node.GetInitialization(), context.scope() });

	auto conditionGroup = generateNext({ context.script(), node.GetCondition(), context.scope() });
	auto bodyGroup = generateNext({ context.script(), node.GetStatement(), context.scope() + 1 });
	auto incrementGroup = generateNext({ context.script(), node.GetIncrement(), context.scope() });

	if (!incrementGroup.value().empty()) {
		conditionGroup.push(Instruction{ Command::JUMP_NIF, conditionGroup.value(), Value { static_cast<long>(bodyGroup.size() + incrementGroup.size() + 1) } });
	}
	incrementGroup.push(Instruction{ Command::JUMP_REL, Value { - static_cast<long>(conditionGroup.size() + bodyGroup.size() + incrementGroup.size() + 1 ) } });

	initGroup.push(std::move(initGroup));
	initGroup.push(std::move(conditionGroup));
	initGroup.push(std::move(bodyGroup));
	initGroup.push(std::move(incrementGroup));
	return initGroup;
}
