#include "InterpreterCommandEnd.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(END)(ExecutionContext& context, Value& left, Value& right) {
	auto outputVar = context.getRelativeInstruction(left.as<long>() + 2);
	return outputVar;
}
