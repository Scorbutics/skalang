#include "NodeValue/AST.h"
#include "MemoryTable.h"
#include "InterpreterOperatorUnary.h"

ska::Token::Variant ska::InterpreterOperator<ska::Operator::UNARY>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	assert(!node.name().empty());
	auto nodeValue = memory[node.name()];
	return nodeValue == nullptr ? "" : *nodeValue;
}

ska::Token::Variant ska::InterpreterOperator<ska::Operator::LITERAL>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	return node.tokenContent();
}