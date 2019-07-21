#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorUnary.h"
#include "Interpreter/Value/Script.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

static ska::BytecodeCellGroup CommonGenerate(ska::BytecodeGenerator& generator, ska::BytecodeGenerationContext& context) {
	/*if (generator.reg().hasCommand()) {
		return {};
	}*/
	auto result = context.cellFromValue(ska::BytecodeCommand::OUT);
	context.script().setReg(result);
	return { std::move(result) };
}

ska::BytecodeCellGroup ska::GeneratorOperator<ska::Operator::UNARY>::generate(OperateOn node, BytecodeGenerationContext& context) {
	return CommonGenerate(m_generator, context);
}

ska::BytecodeCellGroup ska::GeneratorOperator<ska::Operator::LITERAL>::generate(OperateOn node, BytecodeGenerationContext& context) {
	return CommonGenerate(m_generator, context);
}
