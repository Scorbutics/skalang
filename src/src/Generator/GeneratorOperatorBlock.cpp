#include "Config/LoggerConfigLang.h"
#include <string>
#include "GeneratorOperatorBlock.h"
#include "BytecodeCommand.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::GeneratorOperator<ska::Operator::BLOCK>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::GeneratorOperator<ska::Operator::BLOCK>)

ska::BytecodeCellGroup ska::GeneratorOperator<ska::Operator::BLOCK>::generate(OperateOn node, BytecodeGenerationContext& context) {
	auto group = BytecodeCellGroup{};
	for (const auto& child : node) {
		auto childCellGroup = m_generator.generate({ context.script(), *child });
		group.insert(group.end(), childCellGroup.begin(), childCellGroup.end());
	}

	LOG_DEBUG << group;

	return group;
}
