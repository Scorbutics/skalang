#include "InterpreterCommandMov.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MOV)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return context.getCell(left);
}
