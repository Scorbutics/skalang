#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorFor.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FOR_LOOP>::generate(OperateOn node, GenerationContext& context) {

	auto initGroup = generateNext({ context, node.GetInitialization() });

	auto conditionGroup = generateNext({ context, node.GetCondition() });
	auto bodyGroup = generateNext({ context, node.GetStatement(), 1 });
	auto incrementGroup = generateNext({ context, node.GetIncrement() });

	if (!incrementGroup.value().empty()) {
		conditionGroup.push(Instruction{ Command::JUMP_NIF, Value { static_cast<long>(bodyGroup.size() + incrementGroup.size() + 1) }, conditionGroup.value() });
	}
	incrementGroup.push(Instruction{ Command::JUMP_REL, Value { - static_cast<long>(conditionGroup.size() + bodyGroup.size() + incrementGroup.size() + 1 ) } });

	initGroup.push(std::move(initGroup));
	initGroup.push(std::move(conditionGroup));
	initGroup.push(std::move(bodyGroup));
	initGroup.push(std::move(incrementGroup));
	return initGroup;
}
