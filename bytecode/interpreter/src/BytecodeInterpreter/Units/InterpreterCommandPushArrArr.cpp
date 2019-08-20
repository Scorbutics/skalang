#include "InterpreterCommandPushArrArr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(PUSH_ARR_ARR)(ExecutionContext& context, Value& left, Value& right) {
	auto& leftSide = *context.get<NodeValueArray>(left);
	auto& rightSide = *context.get<NodeValueArray>(right);

	auto container = NodeValueArray{};
	container->insert(container->end(), leftSide.begin(), leftSide.end());
	container->insert(container->end(), rightSide.begin(), rightSide.end());
	return container;
}
