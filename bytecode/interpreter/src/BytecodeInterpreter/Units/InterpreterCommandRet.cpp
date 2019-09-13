#include "InterpreterCommandRet.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(RET)(ExecutionContext& context, const Value& left, const Value& right) {
	context.push(context.getCell(context.currentInstruction().dest()));
	context.jumpReturn();
	return {};
}
