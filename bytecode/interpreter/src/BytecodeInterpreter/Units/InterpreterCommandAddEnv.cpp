#include "InterpreterCommandAddEnv.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ADD_ENV)(ExecutionContext& context, const Operand& left, const Operand& right) {
	assert(left.type() == OperandType::VAR || left.type() == OperandType::REG);

	const auto& dest = context.currentInstruction().dest();
	//SLOG(ska::LogLevel::Info) << "\tPushing in env " << left;
	context.pushInEnv(dest, left);
	return {};
}
