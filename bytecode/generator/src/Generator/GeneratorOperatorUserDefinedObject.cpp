#include "Config/LoggerConfigLang.h"
#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorUserDefinedObject.h"
#include "BytecodeCommand.h"
#include "Generator/Value/BytecodeScript.h"
#include "Generator/Value/BytecodeSymbolInfo.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>);
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>)

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::USER_DEFINED_OBJECT>::generate(OperateOn node, GenerationContext& context) {
	auto objectResult = GenerationOutput{ };

	auto fields = std::make_shared<FieldsReferencesRaw>();;

	for(auto& field: node) {
		auto valueGroup = generateNext({ context.script(), *field, context.scope() + 1 });
		fields->emplace(std::get<std::size_t>(valueGroup.value().as<VariableRef>()), fields->size());
		valueGroup.push({Instruction { Command::PUSH, valueGroup.value() }, SymbolInfo { fields }});
		objectResult.push(std::move(valueGroup));
	}

	LOG_INFO << "Creating user defined object : " << objectResult;

	return objectResult;
}
