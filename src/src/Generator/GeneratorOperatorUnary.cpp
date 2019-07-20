#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorUnary.h"
#include "Interpreter/Value/Script.h"
#include "BytecodeCommand.h"

ska::BytecodeCellGroup ska::GeneratorOperator<ska::Operator::UNARY>::generate(OperateOn node, BytecodeGenerationContext& context) {
	return { context.cellFromValue(BytecodeCommand::OUT) };
}

ska::BytecodeCellGroup ska::GeneratorOperator<ska::Operator::LITERAL>::generate(OperateOn node, BytecodeGenerationContext& context) {
	return { context.cellFromValue(BytecodeCommand::OUT) };
}
