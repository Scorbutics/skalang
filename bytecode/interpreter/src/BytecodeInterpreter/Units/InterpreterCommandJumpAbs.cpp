#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandJumpAbs.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::JUMP_ABS>;

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(JUMP_ABS)(ExecutionContext& context, Value& left, Value& right) {
	const auto instructionIndex = static_cast<std::size_t>(context.get<long>(context.currentInstruction().dest()));
	LOG_DEBUG << "Jumping (absolute) to instruction index " << instructionIndex;
	context.jumpAbsolute(instructionIndex);
	return {};
}
