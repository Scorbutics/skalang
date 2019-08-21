#include "InterpreterCommandPop.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(POP)(ExecutionContext& context, Value& left, Value& right) {
	auto container = context.getCell(left);
	context.pop(container);
	return container;
}
