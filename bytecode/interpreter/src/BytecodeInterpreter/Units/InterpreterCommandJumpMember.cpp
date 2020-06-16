#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandJumpMember.h"

using InterpreterCommand = ska::bytecode::InterpreterCommand<ska::bytecode::Command::JUMP_MEMBER>;

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, InterpreterCommand);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, InterpreterCommand)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(JUMP_MEMBER)(ExecutionContext& context, const Operand& left, const Operand& right) {
	const auto& jumpValue = context.currentInstruction().dest();
	const auto data = context.get<NodeValueFunctionMember>(jumpValue);
	auto& thisPrivate = (*data.owner)[0];
	context.push(thisPrivate);
	const auto instructionIndex = data.functionMember->nodeval<ScriptVariableRef>();
	LOG_DEBUG << "Jumping (absolute) to instruction index " << instructionIndex << " in script " << instructionIndex.script;
	context.jumpAbsolute(instructionIndex);
	return {};
}
