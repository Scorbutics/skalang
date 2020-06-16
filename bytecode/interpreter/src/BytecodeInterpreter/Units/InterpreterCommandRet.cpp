#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandRet.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::RET>;

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(RET)(ExecutionContext& context, const Operand& left, const Operand& right) {
	context.push(context.getCell(context.currentInstruction().dest()));
	LOG_DEBUG << "Returning";
	if (context.currentInstruction().dest().type() != OperandType::EMPTY) {
		context.release(context.currentInstruction().dest());
	}
	context.jumpReturn();
	return {};
}
