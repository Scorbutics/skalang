#include <string>
#include "GeneratorOperatorBlock.h"
#include "BytecodeCommand.h"

ska::BytecodeCellGroup ska::GeneratorOperator<ska::Operator::BLOCK>::generate(OperateOn node, BytecodeGenerationContext& context) {
	auto group = BytecodeCellGroup{};
	for (const auto& child : node) {
		auto childCellGroup = m_generator.generate({ context.script(), *child });
		group.insert(group.end(), childCellGroup.begin(), childCellGroup.end());
	}
	return group;
}
