#include "InterpreterCommandUseEnv.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(USE_ENV)(ExecutionContext& context, const Operand& envOperand, const Operand& indexOperand) {
	const auto index = context.get<long>(indexOperand);
	return context.getInEnv(envOperand, index);
}
