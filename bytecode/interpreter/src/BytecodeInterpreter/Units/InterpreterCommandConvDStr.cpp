#include "InterpreterCommandConvDStr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CONV_D_STR)(ExecutionContext& context, Value& left, Value& right) {
  return std::make_shared<std::string>(std::to_string(context.get<double>(left)));
}
