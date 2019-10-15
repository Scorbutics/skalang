#include "InterpreterCommandPushBArr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(PUSH_B_ARR)(ExecutionContext& context, const Value& left, const Value& right) {
  auto toPush = context.getCell(right);
	auto container = context.get<NodeValueArray>(left);
  container->push_back(toPush);
	return container;

}
