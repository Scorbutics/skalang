#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandRet.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::RET>;

SKA_LOGC_CONFIG(ska::LogLevel::Debug, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(RET)(ExecutionContext& context, const Operand& left, const Operand& right) {
	auto returnedValue = context.getCell(context.currentInstruction().dest());
	LOG_DEBUG << "Returning " << returnedValue.convertString();
	if (context.currentInstruction().dest().type() != OperandType::EMPTY) {
		context.release(context.currentInstruction().dest());
	}
	auto returnedEnvNodeValue = context.jumpReturn();
	returnedValue.copyEnv(returnedEnvNodeValue);
	context.push(std::move(returnedValue));
	return {};
}
