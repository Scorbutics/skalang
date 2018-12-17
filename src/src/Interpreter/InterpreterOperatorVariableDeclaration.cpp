#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorVariableDeclaration.h"

ska::Token::Variant ska::InterpreterOperator<ska::Operator::VARIABLE_DECLARATION>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	assert(!node.name().empty());
	auto nodeValue = m_interpreter.interpret(node[0]);
	memory.put(node.name(), nodeValue);
	return nodeValue;
}
