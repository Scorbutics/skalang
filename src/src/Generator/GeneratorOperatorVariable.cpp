#include <string>
#include "GeneratorOperatorVariable.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

static ska::BytecodeCellGroup CommonGenerate(ska::BytecodeGenerator& generator, const ska::ASTNode& node, ska::BytecodeGenerationContext& context) {
	auto contextName = context.pointer().name();
	context.script().setGroup(contextName.empty() ? " " : std::move(contextName));
	auto valueGroup = generator.generate({ context.script(), node });
	valueGroup.push_back(context.cellFromValue(ska::BytecodeCommand::IN));
	context.script().setGroup("");
	return valueGroup;
}

ska::BytecodeCellGroup ska::GeneratorOperator<ska::Operator::VARIABLE_AFFECTATION>::generate(OperateOn node, BytecodeGenerationContext& context) {
	return CommonGenerate(m_generator, node.GetVariableValueNode(), context);
}

ska::BytecodeCellGroup ska::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>::generate(OperateOn node, BytecodeGenerationContext& context) {
	return CommonGenerate(m_generator, node.GetVariableValueNode(), context);
}
