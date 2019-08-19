#include "InterpreterCommandMov.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MOV)(ExecutionContext& context, Value& left, Value& right) {
  return TokenVariantFromValue(context, left);
}
