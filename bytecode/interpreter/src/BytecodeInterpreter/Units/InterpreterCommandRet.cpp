#include "InterpreterCommandRet.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(RET)(ExecutionContext& context, Value& left, Value& right) {
	context.push(context.getCell(context.currentInstruction().dest()));
	return {};
}
