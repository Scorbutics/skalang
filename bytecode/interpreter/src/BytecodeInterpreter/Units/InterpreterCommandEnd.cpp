#include "InterpreterCommandEnd.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(END)(ExecutionContext& context, const Value& left, const Value& right) {
	auto outputVar = context.getRelativeInstruction(left.as<long>() + 1);
	return outputVar;
}
