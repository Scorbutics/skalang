#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorVariableDeclaration.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::VARIABLE_DECLARATION>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	assert(!node.name().empty());
	auto nodeValue = m_interpreter.interpret(node[0]).asRvalue();
	memory.put(node.name(), std::move(nodeValue));
	return "";
}
