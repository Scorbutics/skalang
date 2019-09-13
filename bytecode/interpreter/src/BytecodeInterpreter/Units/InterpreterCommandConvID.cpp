#include "InterpreterCommandConvID.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CONV_I_D)(ExecutionContext& context, const Value& left, const Value& right) {
  return static_cast<double>(context.get<long>(left));
}
