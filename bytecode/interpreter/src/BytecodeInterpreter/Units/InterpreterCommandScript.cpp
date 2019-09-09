#include "InterpreterCommandScript.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(SCRIPT)(ExecutionContext& context, Value& left, Value& right) {
	return context.getCell(context.currentInstruction().left());
}
