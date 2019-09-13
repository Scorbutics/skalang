#include "InterpreterCommandAddStr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ADD_STR)(ExecutionContext& context, const Value& left, const Value& right) {
  return std::make_shared<std::string>(*context.get<StringShared>(left) + *context.get<StringShared>(right));
}
