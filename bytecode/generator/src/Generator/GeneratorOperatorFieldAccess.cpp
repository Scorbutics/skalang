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

	//const auto* fieldReferences = context.script().getSymbolInfo(*symbolField);

	auto objectValue = generateNext({ context.script(), node.GetObjectNameNode(), context.scope()});
	if(objectValue.symbols().empty()) {
		throw std::runtime_error("invalid bytecode : the dereferenced object has no symbol");
	}
	const auto objectFieldReferences = objectValue.symbols().back().references;
	if(objectFieldReferences == nullptr || objectFieldReferences->empty()) {
		throw std::runtime_error("invalid bytecode : the dereferenced object has no fields references");
	}

	const auto fieldRefIndex = (*objectFieldReferences).at(std::get<std::size_t>(fieldValue.as<VariableRef>()));

	LOG_DEBUG << "This field has index " << fieldRefIndex << " in the object";

	objectValue.push({ Instruction { Command::ARR_ACCESS, context.script().queryNextRegister(), objectValue.value(), Value{static_cast<long>(fieldRefIndex)} }});
	return objectValue;
}
