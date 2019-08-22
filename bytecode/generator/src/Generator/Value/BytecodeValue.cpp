#include "BytecodeValue.h"
#include "NodeValue/AST.h"

ska::bytecode::Value::Value(const ASTNode& node) :
  type(ValueType::PURE) {
  switch(node.type().value().type()) {
    case ExpressionType::BOOLEAN:
      content = node.name() == "true";
    break;
    case ExpressionType::FLOAT:
      content = std::stod(node.name());
    break;
    case ExpressionType::INT:
      content = std::stol(node.name());
    break;
    case ExpressionType::STRING:
      content = std::make_shared<std::string>(node.name());
    break;
    case ExpressionType::VOID:
      type = ValueType::EMPTY;
    break;
    default:
      throw std::runtime_error("unsupported conversion");
  }
}

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
	if constexpr (std::is_same_v<VariableRef, TypeT>) {
		output = std::to_string(std::get<std::size_t>(value));
	} else if constexpr (!std::is_same_v<StringShared, TypeT>) {
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
