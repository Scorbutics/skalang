#include "BytecodeValue.h"

std::string ska::bytecode::Value::toString() const {
  if(empty()) {
    return "";
  }

  if(std::holds_alternative<StringShared>(content)) {
    return *std::get<StringShared>(content);
  }

  auto output = std::string {};
  std::visit([&output](const auto& value) {
    using TypeT = std::decay_t<decltype(value)>;
    if constexpr(!std::is_same_v<StringShared, TypeT>) {
      output = std::to_string(value);
    }
  }, content);

  switch(type) {
    case ValueType::VAR:
    return "V" + output;

    case ValueType::REG:
    return "R" + output;

    case ValueType::LBL:
    return "L" + output;

    default:
    break;
  }

  return output;
}
