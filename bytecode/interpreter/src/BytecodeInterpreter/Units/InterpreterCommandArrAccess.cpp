#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandArrAccess.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_ACCESS>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_ACCESS>)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_ACCESS>)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ARR_ACCESS)(ExecutionContext& context, const Value& left, const Value& right) {
  const auto& index = context.get<long>(right);
  const auto& array = (*context.get<NodeValueArray>(left));
  LOG_INFO << "[Accessing cell at index " << index << "/" << array.size() << " of object " << left << "]";

  /*LOG_DEBUG << "Array content : ";
  for(const auto& ac : array) {
    LOG_DEBUG << ac.convertString();
  }*/
  if(index >= array.size()) {
    throw std::runtime_error("invalid array access at index " + std::to_string(index) + " on array size " + std::to_string(array.size()));
  }
  auto result = array[index];
  LOG_INFO << "[Cell content : " << result.convertString() << "]";

  return result;
}
