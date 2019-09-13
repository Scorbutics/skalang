#include "InterpreterCommandCmpArr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CMP_ARR)(ExecutionContext& context, const Value& left, const Value& right) {
  return (*context.get<NodeValueArray>(left) == *context.get<NodeValueArray>(right)) ? 0l : 1l;
}
