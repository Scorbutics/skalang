#include <string>
#include "GeneratorOperatorVariable.h"
#include "BytecodeCommand.h"

ska::BytecodeCellGroup ska::GeneratorOperator<ska::Operator::VARIABLE_AFFECTATION>::generate(OperateOn node, BytecodeGenerationContext& context) {
	auto valueGroup = m_generator.generate({ context.program(), node.GetVariableValueNode() });
	valueGroup.push_back(context.cellFromValue(BytecodeCommand::IN));
	return valueGroup;
}

ska::BytecodeCellGroup ska::GeneratorOperator<ska::Operator::VARIABLE_DECLARATION>::generate(OperateOn node, BytecodeGenerationContext& context) {
	auto valueGroup = m_generator.generate({ context.program(), node.GetVariableValueNode() });
	valueGroup.push_back(context.cellFromValue(BytecodeCommand::IN));
	return valueGroup;
}
