#include "Config/LoggerConfigLang.h"
#include <string>
#include "GeneratorOperatorBlock.h"
#include "BytecodeCommand.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>)

ska::bytecode::ScriptGenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>::generate(OperateOn node, GenerationContext& context) {
	auto group = ScriptGenerationOutput{ };
	for (const auto& child : node) {
		auto childCellGroup = generateNext({ context.script(), *child, context.scope() + 1});
		group.push(std::move(childCellGroup));
	}

	LOG_DEBUG << group;

	return group;
}
