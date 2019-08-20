#include "InterpreterCommandPushFArr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(PUSH_F_ARR)(ExecutionContext& context, Value& left, Value& right) {
  auto toPush = context.getCell(left);
	auto container = context.get<NodeValueArray>(right);
  container->push_back(toPush);
	return container;
}
