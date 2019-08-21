#include "InterpreterCommandJumpAbs.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(JUMP_ABS)(ExecutionContext& context, Value& left, Value& right) {
	context.jumpAbsolute(static_cast<std::size_t>(context.get<long>(left)));
	return {};
}
