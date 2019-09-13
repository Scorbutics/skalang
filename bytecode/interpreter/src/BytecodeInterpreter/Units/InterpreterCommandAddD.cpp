#include "InterpreterCommandAddD.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ADD_D)(ExecutionContext& context, const Value& left, const Value& right) {
  return context.get<double>(left) + context.get<double>(right);
}
