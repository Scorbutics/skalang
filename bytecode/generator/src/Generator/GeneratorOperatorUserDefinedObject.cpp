#include "Config/LoggerConfigLang.h"
#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorUserDefinedObject.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScriptGenerationService.h"
#include "Generator/Value/BytecodeSymbolInfo.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>);
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>)
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>)

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>::generate(OperateOn node, GenerationContext& context) {
	auto objectResult = ScriptGenerationOutput{ };

	auto fields = std::make_shared<FieldsReferencesRaw>();

	std::size_t childIndex = 0;
	for(auto& field: node) {
		auto valueGroup = generateNext({ context, *field, 1 });
		auto fieldRef = valueGroup.value().as<ScriptVariableRef>();
		fields->emplace(std::move(fieldRef), fields->size());

		auto symbolInfo = SymbolInfo { fields, context.scriptIndex() };
		symbolInfo.priority = childIndex;
		LOG_INFO << "Registering symbol info " << symbolInfo << " for field node " << *field;

		context.setSymbolInfo(*field, std::move(symbolInfo));

		valueGroup.push(Instruction { Command::PUSH, valueGroup.value() });
		objectResult.push(std::move(valueGroup));
		childIndex++;
	}

	LOG_INFO << "Creating user defined object ";
	LOG_DEBUG << objectResult;

	return objectResult;
}
