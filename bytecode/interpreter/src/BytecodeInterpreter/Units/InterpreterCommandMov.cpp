#include "InterpreterCommandMov.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MOV)(ExecutionContext& context, const Value& left, const Value& right) {
  return context.getCell(left);
}
