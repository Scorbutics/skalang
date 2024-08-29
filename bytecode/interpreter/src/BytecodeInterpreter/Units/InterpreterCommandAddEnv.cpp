#include "InterpreterCommandAddEnv.h"

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ADD_ENV)(ExecutionContext& context, const Operand& left, const Operand& right) {
	const auto& variable = context.currentInstruction().dest();
	assert(variable.type() == OperandType::VAR || variable.type() == OperandType::REG);

	auto addedVariable = context.getCell(variable);
	context.pushInEnv(variable.as<ScriptVariableRef>(), std::move(addedVariable));
	return {};
}
