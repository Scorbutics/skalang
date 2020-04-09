#include "Config/LoggerConfigLang.h"
#include <string>
#include "GeneratorOperatorBlock.h"
#include "BytecodeCommand.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>)

ska::bytecode::InstructionOutput ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>::generate(OperateOn node, GenerationContext& context) {
	auto group = InstructionOutput{ };

	//auto fields = std::make_shared<FieldsReferencesRaw>();

	std::size_t childIndex = 0;
	for (const auto& child : node) {
		auto childCellGroup = generateNext({ context, *child, 1});
		if(child->isSymbolicLeaf() && childCellGroup.operand().type() == OperandType::VAR) {
			/*auto fieldRef = childCellGroup.operand().as<ScriptVariableRef>();
			fields->emplace(std::move(fieldRef), fields->size());*/

			auto* oldSymbolInfo = context.getSymbolInfo(*child);
			auto symbolInfo = SymbolInfo { context.scope() + 1, child->name(), std::make_shared<FieldsReferencesRaw>(), context.scriptIndex() };
			if (oldSymbolInfo != nullptr) {
				symbolInfo.binding = oldSymbolInfo->binding;
				symbolInfo.bindingPassThrough = oldSymbolInfo->bindingPassThrough;
			}
			if (context.scope() == 0) {
				symbolInfo.exported = true;
				symbolInfo.childIndex = childIndex;
			}
			LOG_INFO << "%12cRegistering symbol info " << symbolInfo << " for field node " << *child << " (this symbol is " << (context.scope() == 0 ? "exported" : "not exported") << ")";
			context.setSymbolInfo(*child, std::move(symbolInfo));
		}
		group.push(std::move(childCellGroup));
		childIndex++;
	}

	LOG_DEBUG << group;

	return group;
}
