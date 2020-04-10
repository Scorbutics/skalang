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

	objectValue.push({ Instruction { Command::ARR_ACCESS, context.queryNextRegister(), objectValue.operand(), Operand {static_cast<long>(index), OperandType::PURE } }});
	return objectValue;
}
