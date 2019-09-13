#include "InterpreterCommandDivD.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(DIV_D)(ExecutionContext& context, const Value& left, const Value& right) {
  return context.get<double>(left) / context.get<double>(right);
}
