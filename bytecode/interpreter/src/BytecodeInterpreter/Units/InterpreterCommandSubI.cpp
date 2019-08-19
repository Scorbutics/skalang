#include "InterpreterCommandSubI.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(SUB_I)(ExecutionContext& context, Value& left, Value& right) {
  return context.get<long>(left) - context.get<long>(right);
}
