#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandClearRange.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::InterpreterCommand<ska::bytecode::Command::CLEAR_RANGE>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::InterpreterCommand<ska::bytecode::Command::CLEAR_RANGE>)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::InterpreterCommand<ska::bytecode::Command::CLEAR_RANGE>)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(CLEAR_RANGE)(ExecutionContext& context, const Operand& left, const Operand& right) {
  const auto indexStart = context.currentInstruction().dest().as<ScriptVariableRef>().variable;
  const auto indexEnd = left.as<ScriptVariableRef>().variable;

  auto type = context.currentInstruction().dest().type();
  auto script = context.currentInstruction().dest().as<ScriptVariableRef>().script;
  
  for (auto i = indexStart; i < indexEnd; i++) {
      context.release(Operand{ ScriptVariableRef{i, script}, type, {}});
  }

  return {};
}
