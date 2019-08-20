#include "InterpreterCommandCmpStr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CMP_STR)(ExecutionContext& context, Value& left, Value& right) {
  return (*context.get<StringShared>(left) == *context.get<StringShared>(right)) ? 0l : 1l;
}
