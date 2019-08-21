#include "InterpreterCommandJumpRel.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(JUMP_REL)(ExecutionContext& context, Value& left, Value& right) {
	context.jumpRelative(context.get<long>(left));
	return {};
}
