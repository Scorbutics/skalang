#include "InterpreterCommandLabel.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(LABEL)(ExecutionContext& context, Value& left, Value& right) {
	return context.markLabel();
}
