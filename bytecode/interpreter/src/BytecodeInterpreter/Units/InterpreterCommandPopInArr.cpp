#include "InterpreterCommandPopInArr.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(POP_IN_ARR)(ExecutionContext& context, const Value& left, const Value& right) {
	auto result = std::make_shared<NodeValueArrayRaw>();
	context.pop(*result, context.get<long>(left));
	return result;
}
