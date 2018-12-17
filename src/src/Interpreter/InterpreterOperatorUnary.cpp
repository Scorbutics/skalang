#include <string>
#include "NodeValue/AST.h"
#include "MemoryTable.h"
#include "InterpreterOperatorUnary.h"

namespace ska {
    Token::Variant InterpreterOperatorConvertString(ExpressionType type, const Token::Variant& value) {
        if(type == ExpressionType::INT) {
            return std::stoi(std::get<std::string>(value));
        } else if(type == ExpressionType::FLOAT) {
            return std::stof(std::get<std::string>(value));
        }
        return std::get<std::string>(value);
    }
}

ska::Token::Variant ska::InterpreterOperator<ska::Operator::UNARY>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	assert(!node.name().empty());
	auto nodeValue = memory[node.name()];
    if(nodeValue == nullptr) {
        return "";
    }

    return InterpreterOperatorConvertString(node.type().value().type(), *nodeValue);
}

ska::Token::Variant ska::InterpreterOperator<ska::Operator::LITERAL>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	return InterpreterOperatorConvertString(node.type().value().type(), node.tokenContent());
}
