#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorFieldAccess.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>)

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>::generate(OperateOn node, GenerationContext& context) {
	auto objectPath = generateNext({context.script(), node.GetObjectNameNode(), context.scope() });
	auto fieldResult = generateNext({context.script(), node.GetFieldNameNode(), context.scope() });

	LOG_DEBUG << "Accessing field " << fieldResult << " of object " << objectPath;

	//objectPath.push(std::move(fieldResult));

	return fieldResult;
}
