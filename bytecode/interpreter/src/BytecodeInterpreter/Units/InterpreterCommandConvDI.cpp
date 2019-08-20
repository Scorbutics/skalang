#include "InterpreterCommandConvDI.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CONV_D_I)(ExecutionContext& context, Value& left, Value& right) {
  return static_cast<long>(context.get<double>(left));
}
