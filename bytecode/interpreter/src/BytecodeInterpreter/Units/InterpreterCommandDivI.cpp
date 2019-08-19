#include "InterpreterCommandDivI.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(DIV_I)(ExecutionContext& context, const Value& left, const Value& right) {
  return context.get<long>(left) / context.get<long>(right);
}
