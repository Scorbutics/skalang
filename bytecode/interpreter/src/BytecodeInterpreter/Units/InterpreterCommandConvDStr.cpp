#include "InterpreterCommandConvDStr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CONV_D_STR)(ExecutionContext& context, const Operand& left, const Operand& right) {
  return std::make_shared<std::string>(std::to_string(context.get<double>(left)));
}
