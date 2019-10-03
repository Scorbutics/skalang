#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorIf.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::IF>::generate(OperateOn node, GenerationContext& context) {
	auto conditionGroup = generateNext({ context, node.GetCondition() });
	auto ifGroup = generateNext({ context, node.GetIfStatement(), 1 });
	conditionGroup.push(Instruction{ Command::JUMP_NIF, Value { static_cast<long>(ifGroup.size()) }, conditionGroup.value() });
	conditionGroup.push(std::move(ifGroup));
	return conditionGroup;
}

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::IF_ELSE>::generate(OperateOn node, GenerationContext& context) {
	auto conditionGroup = generateNext({ context, node.GetCondition() });
	auto ifGroup = generateNext({ context, node.GetIfStatement(), 1 });
	auto elseGroup = generateNext({ context, node.GetElseStatement(), 1 });

	ifGroup.push(Instruction{ Command::JUMP_REL, Value { static_cast<long>(elseGroup.size()) } });

	conditionGroup.push(Instruction{ Command::JUMP_NIF, Value { static_cast<long>(ifGroup.size()) }, conditionGroup.value()  });
	conditionGroup.push(std::move(ifGroup));
	conditionGroup.push(std::move(elseGroup));
	return conditionGroup;
}
