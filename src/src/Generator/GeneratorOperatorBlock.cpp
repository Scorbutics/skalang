#include "Config/LoggerConfigLang.h"
#include <string>
#include "GeneratorOperatorBlock.h"
#include "BytecodeCommand.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>)

ska::bytecode::GenerationOutput ska::bytecode::GeneratorOperator<ska::Operator::BLOCK>::generate(OperateOn node, GenerationContext& context) {
	auto group = GenerationOutput{ InstructionPack{} };
	for (const auto& child : node) {
		auto childCellGroup = m_generator.generate({ context.script(), *child });
		group.push(std::move(childCellGroup));
	}

	LOG_DEBUG << group.pack();

	return group;
}
