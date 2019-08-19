#include "InterpreterCommandMulI.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MUL_I) {
  const auto& instruction = context.currentInstruction();
  context.set(instruction.dest(), instruction.left().as<long>() * instruction.right().as<long>());
  return {};
}
