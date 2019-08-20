#include "InterpreterCommandMulIArr.h"
#include "InterpreterCommandMulArrI.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(MUL_I_ARR)(ExecutionContext& context, Value& left, Value& right) {
  return InterpreterCommand<Command::MUL_I_ARR>{m_interpreter}.interpret(context, right, left);
}
