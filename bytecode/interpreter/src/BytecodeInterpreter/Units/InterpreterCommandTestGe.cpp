#include "InterpreterCommandTestGe.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(TEST_GE)(ExecutionContext& context, const Value& left, const Value& right) {
  return context.get<long>(left) >= 0;
}
