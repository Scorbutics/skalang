#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorReturn.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::RETURN>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::RETURN>)

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::RETURN>::generate(OperateOn node, GenerationContext& context) {
	auto objectResult = generateNext({context, node.GetValue() });

	LOG_DEBUG << "Returning " << node.GetValue().size() << " fields";

	if(node.GetValue().op() == Operator::USER_DEFINED_OBJECT)  {
		auto registerV = context.script().queryNextRegister();
		const auto numberOfItemsReturned = static_cast<long>(node.GetValue().size());
		objectResult.push(Instruction { Command::POP_IN_VAR, registerV, Value { numberOfItemsReturned } });
	}

	LOG_DEBUG << "Returning : " << objectResult;

	return objectResult;
}
