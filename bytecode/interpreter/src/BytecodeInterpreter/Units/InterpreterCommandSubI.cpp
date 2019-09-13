#include "InterpreterCommandSubI.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(SUB_I)(ExecutionContext& context, const Value& left, const Value& right) {
  return context.get<long>(left) - context.get<long>(right);
}
