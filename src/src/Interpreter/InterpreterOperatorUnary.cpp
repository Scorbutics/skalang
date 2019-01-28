#include <string>
#include "NodeValue/AST.h"
#include "MemoryTable.h"
#include "InterpreterOperatorUnary.h"

namespace ska {
	NodeValue InterpreterOperatorConvertString(ExpressionType type, const Token::Variant& value) {
        switch(type) {
		case ExpressionType::INT:
            return std::holds_alternative<int>(value) ? value : std::stoi(std::get<std::string>(value));
		case ExpressionType::FLOAT:
            return std::holds_alternative<double>(value) ? value : std::stof(std::get<std::string>(value));
		case ExpressionType::BOOLEAN:
			return std::holds_alternative<bool>(value) ? value : (std::get<std::string>(value) == "true");
		default:
			return std::get<std::string>(value);
        }
    }
}

ska::NodeCell ska::InterpreterOperator<ska::Operator::UNARY>::interpret(OperateOn node) {
	assert(!node.GetValue().empty());
	auto nodeValue = m_memory[node.GetValue()];
    if(nodeValue.first == nullptr) {
        return NodeCell {""};
    }

	return nodeValue; //InterpreterOperatorConvertString(node.type().value().type(), std::get<Token::Variant>(*nodeValue));
}

ska::NodeCell ska::InterpreterOperator<ska::Operator::LITERAL>::interpret(OperateOn node) {
	return InterpreterOperatorConvertString(node.GetType().value().type(), node.GetValue());
}
