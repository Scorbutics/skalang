#include "InterpreterCommandMulD.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MUL_D)(ExecutionContext& context, Value& left, Value& right) {
  return context.get<double>(left) * context.get<double>(right);
}
