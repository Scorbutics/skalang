#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandArrAccess.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_ACCESS>);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_ACCESS>)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ARR_ACCESS)(ExecutionContext& context, Value& left, Value& right) {
  const auto& index = context.get<long>(right);

  LOG_DEBUG << "Accessing cell at index " << index << " of object " << left;
  const auto& array = (*context.get<NodeValueArray>(left));
  LOG_DEBUG << "Array size : " << array.size() << " and content : ";
  for(const auto& ac : array) {
    LOG_DEBUG << ac.convertString();
  }
  auto result = array[index];
  LOG_DEBUG << "Output : " << result.convertString();

  return result;
}
