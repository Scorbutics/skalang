#include "InterpreterCommandPush.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(PUSH)(ExecutionContext& context, const Value& left, const Value& right) {
	auto un = context.getCell(context.currentInstruction().dest());
	auto dos = context.getCell(left);
	auto tres = context.getCell(right);

	context.push(un, dos, tres);
	return {};
}