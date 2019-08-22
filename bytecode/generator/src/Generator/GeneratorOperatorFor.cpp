#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorFor.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FOR_LOOP>::generate(OperateOn node, GenerationContext& context) {

	auto initGroup = m_generator.generate({ context.script(), node.GetInitialization() });

	auto conditionGroup = m_generator.generate({ context.script(), node.GetCondition() });
	auto bodyGroup = m_generator.generate({ context.script(), node.GetStatement() });
	auto incrementGroup = m_generator.generate({ context.script(), node.GetIncrement() });

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
