#include "InterpreterCommandConvID.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CONV_I_D)(ExecutionContext& context, Value& left, Value& right) {
  return static_cast<double>(context.get<long>(left));
}
