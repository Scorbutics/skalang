#include "InterpreterCommandEnd.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(END)(ExecutionContext& context, Value& left, Value& right) {
	auto label = context.getCell(left);
	context.push(context.getCell(right));
	return label;
}
