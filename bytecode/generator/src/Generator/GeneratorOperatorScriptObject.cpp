#include "Config/LoggerConfigLang.h"
#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorScriptObject.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationHelper.h"
#include "Generator/Value/BytecodeSymbolInfo.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::SCRIPT_OBJECT>);
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::GeneratorOperator<ska::Operator::SCRIPT_OBJECT>)
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::SCRIPT_OBJECT>)

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::SCRIPT_OBJECT>::generate(OperateOn node, GenerationContext& context) {
	auto objectResult = InstructionOutput{ };

	std::size_t childIndex = 0;
	for(auto& field: node) {
		auto valueGroup = generateNext(context.next(*field, 1));

		auto* oldSymbolInfo = context.getSymbolInfo(*field);
		auto symbolInfo = SymbolInfo { context.scope() + 1, field->name(), context.scriptIndex() };
		if (oldSymbolInfo != nullptr) {
			symbolInfo.binding = oldSymbolInfo->binding;
			symbolInfo.bindingPassThrough = oldSymbolInfo->bindingPassThrough;
		}
		symbolInfo.childIndex = childIndex;
		symbolInfo.exported = true;
		LOG_INFO << "%12cRegistering symbol info " << symbolInfo << " for field node " << *field << " (this symbol is not exported)";

		context.setSymbolInfo(*field, std::move(symbolInfo));

		if (valueGroup.operand().type() == OperandType::VAR) {
			valueGroup.push(Instruction { Command::PUSH, valueGroup.operand() });
		}
		objectResult.push(std::move(valueGroup));
		childIndex++;
	}

	LOG_INFO << "Creating user defined object ";
	LOG_DEBUG << objectResult;

	return objectResult;
}
