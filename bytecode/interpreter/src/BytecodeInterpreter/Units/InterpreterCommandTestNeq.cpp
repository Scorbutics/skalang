#include "InterpreterCommandTestNeq.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(TEST_NEQ)(ExecutionContext& context, Value& left, Value& right) {
  return context.get<long>(left) != 0;
}
