#include "InterpreterCommandUseEnv.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(USE_ENV)(ExecutionContext& context, const Operand& left, const Operand& right) {
	/*const auto& variable = context.currentInstruction().dest();
	assert(variable.type() == OperandType::VAR || variable.type() == OperandType::REG);

	auto* value = context.useFromCurrentEnv(variable.as<ScriptVariableRef>());
	assert(value != nullptr);*/
	return {};
}
