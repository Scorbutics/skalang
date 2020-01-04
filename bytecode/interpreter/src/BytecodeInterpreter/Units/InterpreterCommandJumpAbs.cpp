#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandJumpAbs.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::JUMP_ABS>;

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(JUMP_ABS)(ExecutionContext& context, const Operand& left, const Operand& right) {
	const auto& jumpValue = context.currentInstruction().dest();
	const auto instructionIndex = context.get<ScriptVariableRef>(jumpValue);
	LOG_DEBUG << "Jumping (absolute) to instruction index " << instructionIndex << " in script " << instructionIndex.script;
	context.jumpAbsolute(instructionIndex);
	return {};
}
