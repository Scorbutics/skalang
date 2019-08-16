#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorReturn.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::RETURN>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::RETURN>)

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::RETURN>::generate(OperateOn node, GenerationContext& context) {
	auto objectResult = m_generator.generate({context.script(), node.GetValue()});

	if(node.GetValue().type() == ExpressionType::OBJECT) {
		auto ss = std::stringstream{};
		ss << node.GetValue().size();
		auto registerV = context.script().queryNextRegister();
		objectResult.push(Instruction { Command::POP_IN_VAR, registerV, Value { ss.str() } });
	}

	LOG_DEBUG << "Returning : " << objectResult ;

	return objectResult;
}
