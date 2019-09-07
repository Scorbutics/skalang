#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandJumpAbs.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::JUMP_ABS>;

SKA_LOGC_CONFIG(ska::LogLevel::Debug, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(JUMP_ABS)(ExecutionContext& context, Value& left, Value& right) {
	const auto& jumpValue = context.currentInstruction().dest();
	const auto instructionIndex = context.get<std::size_t>(jumpValue);
	LOG_DEBUG << "Jumping (absolute) to instruction index " << instructionIndex;
	context.jumpAbsolute(instructionIndex);
	return {};
}
