#include "InterpreterCommandEnd.h"
#include "Config/LoggerConfigLang.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::END>;


SKA_LOGC_CONFIG(ska::LogLevel::Debug, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)


SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(END)(ExecutionContext& context, const Operand& left, const Operand& right) {
	auto outputVar = context.getRelativeInstruction(left.as<long>() + 1);
	LOG_DEBUG << "End of function " << outputVar;
	return outputVar;
}
