#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorReturn.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::RETURN>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::RETURN>)

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::RETURN>::generate(OperateOn node, GenerationContext& context) {
	auto objectResult = generateNext({context.script(), node.GetValue(), context.scope() });

	if(node.GetValue().type() == ExpressionType::OBJECT) {
		auto registerV = context.script().queryNextRegister();
		//registerV.setReferencesFrom(objectResult);
		objectResult.push(Instruction { Command::POP_IN_VAR, registerV, Value { static_cast<long>(node.GetValue().size()) } });
	}

	LOG_DEBUG << "Returning : " << objectResult ;

	return objectResult;
}
