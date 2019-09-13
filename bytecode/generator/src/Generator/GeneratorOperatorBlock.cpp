#include "Config/LoggerConfigLang.h"
#include <string>
#include "GeneratorOperatorBlock.h"
#include "BytecodeCommand.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>)

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>::generate(OperateOn node, GenerationContext& context) {
	auto group = ScriptGenerationOutput{ };

	auto fields = std::make_shared<FieldsReferencesRaw>();;

	std::size_t childIndex = 0;
	for (const auto& child : node) {
		auto childCellGroup = generateNext({ context, *child, 1});
		if(child->symbol() != nullptr && childCellGroup.value().type() == ValueType::VAR) {
			fields->emplace(std::get<std::size_t>(childCellGroup.value().as<VariableRef>()), fields->size());

			auto symbolInfo = SymbolInfo { fields, context.scriptIndex() };
			if (context.scope() == 0) {
				symbolInfo.exported = true;
				symbolInfo.priority = childIndex;
			}
			LOG_INFO << "Registering symbol info " << symbolInfo << " for field node " << *child << " (this symbol is " << (context.scope() == 0 ? "exported" : "not exported") << ")";
			context.setSymbolInfo(*child, std::move(symbolInfo));
		}
		group.push(std::move(childCellGroup));
		childIndex++;
	}

	//context.setSymbolInfo(node.asNode(), SymbolInfo { fields });

	LOG_DEBUG << group;

	return group;
}
