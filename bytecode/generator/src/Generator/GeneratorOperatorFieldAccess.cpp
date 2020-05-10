#include "Config/LoggerConfigLang.h"
#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorFieldAccess.h"
#include "NodeValue/ScriptAST.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>)

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::FIELD_ACCESS>::generate(OperateOn node, GenerationContext& context) {
	const auto& fieldName = node.GetFieldNameNode().name();

	const auto* objectTypeSymbol = node.GetObjectNameNode().typeSymbol();
	if (objectTypeSymbol == nullptr) {
		throw std::runtime_error("unable to retrieve object type");
	}

	// Handling built-in direct fields
	const auto typeObject = node.GetObjectType();
	if (typeObject == ExpressionType::ARRAY) {
		if (fieldName == "size") {
			auto objectValue = generateNext({ context, node.GetObjectNameNode() });
			return { Instruction{ Command::ARR_LENGTH, context.queryNextRegister(), objectValue.operand() } };
		}
		auto ss = std::stringstream{};
		ss << "trying to access an undeclared built-in field \"" << fieldName << "\" of the type \"" << typeObject << "\"";
		throw std::runtime_error(ss.str());
	}

	// Handling custom types
	const auto* symbolField = (*objectTypeSymbol)(fieldName);
	if (symbolField == nullptr) {
		auto ss = std::stringstream{};
		ss << "trying to access to an undeclared field : \"" << fieldName << "\" of \"" << node.GetObjectNameNode().name() << "\"";
		throw std::runtime_error(ss.str());
	}

	const auto* objectSymbolInfo = context.getSymbolInfo(*symbolField);
	if (objectSymbolInfo == nullptr) {
		auto ss = std::stringstream { };
		ss << "invalid bytecode : the dereferenced object \"" << node.GetObjectNameNode() << "\" is not registered in script ";
		ss << context.scriptName();
		throw std::runtime_error(ss.str());
	}

	std::size_t index;
	if (objectSymbolInfo->exported) {
		index = context.exportId(*symbolField);
	} else {
		index = objectTypeSymbol->id(*symbolField);
	}

	auto objectValue = generateNext({ context, node.GetObjectNameNode()});

	LOG_DEBUG << "This field has index " << index << " in the object";
	
	auto command = Command::ARR_ACCESS;
	if (symbolField->type() == ExpressionType::FUNCTION && symbolField->type()[0].name() == "this.private.fcty") {
		command = Command::ARR_MEMBER_ACCESS;
	}

	objectValue.push({ Instruction { command, context.queryNextRegister(), objectValue.operand(), Operand {static_cast<long>(index), OperandType::PURE } }});
	return objectValue;
}
