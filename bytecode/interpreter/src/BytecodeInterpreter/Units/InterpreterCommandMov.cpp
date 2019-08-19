#include "InterpreterCommandMov.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MOV) {
  const auto& instruction = context.currentInstruction();
  context.set(instruction.dest(), TokenVariantFromValue(instruction.left()));
  return {};
}
