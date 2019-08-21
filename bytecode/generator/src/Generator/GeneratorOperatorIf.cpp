#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorIf.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::IF>::generate(OperateOn node, GenerationContext& context) {
	auto conditionGroup = m_generator.generate({ context.script(), node.GetCondition() });
	auto ifGroup = m_generator.generate({ context.script(), node.GetIfStatement() });
	conditionGroup.push(Instruction{ Command::JUMP_NIF, conditionGroup.value(), Value { ifGroup.size() } });
	conditionGroup.push(std::move(ifGroup));
	return conditionGroup;
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::IF_ELSE>::generate(OperateOn node, GenerationContext& context) {
	auto conditionGroup = m_generator.generate({ context.script(), node.GetCondition() });
	auto ifGroup = m_generator.generate({ context.script(), node.GetIfStatement() });
	auto elseGroup = m_generator.generate({ context.script(), node.GetElseStatement() });

	ifGroup.push(Instruction{ Command::JUMP_REL, Value { elseGroup.size() } });

	conditionGroup.push(Instruction{ Command::JUMP_NIF, conditionGroup.value(), Value { ifGroup.size() } });
	conditionGroup.push(std::move(ifGroup));
	conditionGroup.push(std::move(elseGroup));
	return conditionGroup;
}
