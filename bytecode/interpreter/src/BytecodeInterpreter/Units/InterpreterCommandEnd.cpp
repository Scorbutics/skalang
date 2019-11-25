#include "InterpreterCommandEnd.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(END)(ExecutionContext& context, const Operand& left, const Operand& right) {
	auto outputVar = context.getRelativeInstruction(left.as<long>() + 1);
	return outputVar;
}
