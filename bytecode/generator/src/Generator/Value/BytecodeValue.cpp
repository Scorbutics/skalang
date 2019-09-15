#include "BytecodeValue.h"
#include "NodeValue/AST.h"

ska::bytecode::Value::Value(const ASTNode& node) :
  m_type(ValueType::PURE) {
  switch(node.type().value().type()) {
    case ExpressionType::BOOLEAN:
      m_content = node.name() == "true";
    break;
    case ExpressionType::FLOAT:
      m_content = std::stod(node.name());
    break;
    case ExpressionType::INT:
      m_content = std::stol(node.name());
    break;
    case ExpressionType::STRING:
      m_content = std::make_shared<std::string>(node.name());
    break;
    case ExpressionType::VOID:
      m_type = ValueType::EMPTY;
    break;
    default: {
      auto ss = std::stringstream {};
      ss << "unsupported conversion of node type \"" << node.type().value() << "\"";
      throw std::runtime_error(ss.str());
    }
  }
}

std::string ska::bytecode::Value::toString() const {
  if(empty()) {
    return "";
  }

  if(std::holds_alternative<StringShared>(m_content)) {
    return *std::get<StringShared>(m_content);
  }

  auto output = std::string {};
  std::visit([&output](const auto& value) {
    using TypeT = std::decay_t<decltype(value)>;
	if constexpr (std::is_same_v<VariableRef, TypeT>) {
		output = std::to_string(value.variable) /*+ ":" + std::to_string(value.script)*/;
	} else if constexpr (!std::is_same_v<StringShared, TypeT>) {
      output = std::to_string(value);
    }
  }, m_content);


  switch(m_type) {
    case ValueType::VAR:
    output = "V" + output;
    break;

    case ValueType::REG:
    output = "R" + output;
    break;

    default:
    break;
  }

  return output;
}

bool ska::bytecode::operator==(const Value& lhs, const Value& rhs) {
	return lhs.m_content == rhs.m_content && lhs.m_type == rhs.m_type;
}

bool ska::bytecode::operator==(const ScriptVariableRef& lhs, const ScriptVariableRef& rhs) {
	return lhs.script == rhs.script && lhs.variable == rhs.variable;
}

bool ska::bytecode::operator==(const VariableRef& lhs, const VariableRef& rhs) {
	return lhs.variable == rhs.variable;
}
