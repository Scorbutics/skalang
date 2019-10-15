#include "InterpreterCommandPop.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(POP)(ExecutionContext& context, const Value& left, const Value& right) {
	auto container = context.getCell(left);
	context.pop(container);
	return container;
}
