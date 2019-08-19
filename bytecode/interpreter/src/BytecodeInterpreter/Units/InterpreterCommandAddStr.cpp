#include "InterpreterCommandAddStr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ADD_STR)(ExecutionContext& context, Value& left, Value& right) {
  return std::make_shared<std::string>(*context.get<StringShared>(left) + *context.get<StringShared>(right));
}
