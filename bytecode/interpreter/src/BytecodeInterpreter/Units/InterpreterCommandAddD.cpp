#include "InterpreterCommandAddD.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ADD_D)(ExecutionContext& context, Value& left, Value& right) {
  return context.get<double>(left) + context.get<double>(right);
}
