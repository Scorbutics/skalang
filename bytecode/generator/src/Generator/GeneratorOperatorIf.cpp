#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorIf.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::IF>::generate(OperateOn node, GenerationContext& context) {
	auto conditionGroup = generateNext({ context.script(), node.GetCondition(), context.scope() });
	auto ifGroup = generateNext({ context.script(), node.GetIfStatement(), context.scope() + 1 });
	conditionGroup.push(Instruction{ Command::JUMP_NIF, conditionGroup.value(), Value { static_cast<long>(ifGroup.size()) } });
	conditionGroup.push(std::move(ifGroup));
	return conditionGroup;
}

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::IF_ELSE>::generate(OperateOn node, GenerationContext& context) {
	auto conditionGroup = generateNext({ context.script(), node.GetCondition(), context.scope() });
	auto ifGroup = generateNext({ context.script(), node.GetIfStatement(), context.scope() + 1 });
	auto elseGroup = generateNext({ context.script(), node.GetElseStatement(), context.scope() + 1 });

	ifGroup.push(Instruction{ Command::JUMP_REL, Value { static_cast<long>(elseGroup.size()) } });

	conditionGroup.push(Instruction{ Command::JUMP_NIF, conditionGroup.value(), Value { static_cast<long>(ifGroup.size()) } });
	conditionGroup.push(std::move(ifGroup));
	conditionGroup.push(std::move(elseGroup));
	return conditionGroup;
}
