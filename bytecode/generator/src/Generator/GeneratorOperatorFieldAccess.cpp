#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorFieldAccess.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>)

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>::generate(OperateOn node, GenerationContext& context) {
	const auto typeObject = node.GetObjectType();
	const auto& fieldName = node.GetFieldNameNode().name();
	const auto* symbolField = typeObject[fieldName];
	if (symbolField == nullptr || !typeObject.hasSymbol()) {
		auto ss = std::stringstream{};
		ss << "trying to access to an undeclared field : \"" << fieldName << "\" of \"" << node.GetObjectNameNode().name() << "\"";
        throw std::runtime_error(ss.str());
	}

	auto fieldValue = context.script().querySymbol(*symbolField);

	LOG_DEBUG << "Accessing field " << fieldValue << " of object " << node.GetObjectNameNode().name();

	//auto indexValue = context.script().fieldIndex(fieldValue);

	//return Instruction { Command::ARR_ACCESS, std::move(objectValue), std::move(indexValue) };
	return fieldValue;
}
