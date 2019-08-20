#include "InterpreterCommandPopInArr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(POP_IN_ARR)(ExecutionContext& context, Value& left, Value& right) {
	auto result = std::make_shared<NodeValueArrayRaw>();
	context.pop(*result);
	return result;
}
