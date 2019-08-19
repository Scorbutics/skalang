#include "InterpreterCommandPushArrArr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(PUSH_ARR_ARR)(ExecutionContext& context, Value& left, Value& right) {
	auto& leftSide = *context.getVariant(left).as<NodeValueArray>();
	auto& rightSide = *context.getVariant(right).as<NodeValueArray>();

	auto container = NodeValueArray{};
	container->insert(container->end(), std::make_move_iterator(leftSide.begin()), std::make_move_iterator(leftSide.end()));
	container->insert(container->end(), std::make_move_iterator(rightSide.begin()), std::make_move_iterator(rightSide.end()));
	return container;
}
