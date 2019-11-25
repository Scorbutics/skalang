#include "Config/LoggerConfigLang.h"
#include "InterpreterCommandArrAccess.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_ACCESS>);
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_ACCESS>)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::InterpreterCommand<ska::bytecode::Command::ARR_ACCESS>)

SKALANG_BYTECODE_INTERPRETER_COMMAND_DECLARE(ARR_ACCESS)(ExecutionContext& context, const Operand& left, const Operand& right) {
  assert(left.type() == ValueType::VAR || left.type() == ValueType::REG);
  assert(right.type() == ValueType::PURE);

  LOG_DEBUG << "Index cell node " << right << " of type " << right.type() << " of object \"" << context.getCell(left).convertString() << "\" of type " << left.type();

  const auto index = context.get<long>(right);
  auto array = context.get<NodeValueArray>(left);
  LOG_INFO << "[Accessing cell at index " << index << "/" << array->size() << " of object " << left << "]";

  if(index >= array->size()) {
    throw std::runtime_error("invalid array access at index " + std::to_string(index) + " on array size " + std::to_string(array->size()));
  }
  auto& result = (*array)[index];
  LOG_INFO << "[Cell content : " << result.convertString() << "]";

  return &result;
}
