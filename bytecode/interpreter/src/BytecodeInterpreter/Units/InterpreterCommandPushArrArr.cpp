#include "InterpreterCommandPushArrArr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(PUSH_ARR_ARR)(ExecutionContext& context, Value& left, Value& right) {
	auto& leftSide = *context.get<NodeValueArray>(left);
	auto& rightSide = *context.get<NodeValueArray>(right);

	auto container = NodeValueArray{};
	container->insert(container->end(), std::make_move_iterator(leftSide.begin()), std::make_move_iterator(leftSide.end()));
	container->insert(container->end(), std::make_move_iterator(rightSide.begin()), std::make_move_iterator(rightSide.end()));
	return container;
}
