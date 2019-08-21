#include "InterpreterCommandPopInVar.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(POP_IN_VAR)(ExecutionContext& context, Value& left, Value& right) {
	auto result = std::make_shared<NodeValueArrayRaw>();
	context.pop(*result, context.get<long>(left));
	return result;
}
