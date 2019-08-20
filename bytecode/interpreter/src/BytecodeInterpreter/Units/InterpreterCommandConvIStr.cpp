#include "InterpreterCommandConvIStr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CONV_I_STR)(ExecutionContext& context, Value& left, Value& right) {
  return std::make_shared<std::string>(std::to_string(context.get<long>(left)));
}
