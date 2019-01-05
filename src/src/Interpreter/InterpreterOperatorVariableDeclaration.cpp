#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorVariableDeclaration.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::VARIABLE_DECLARATION>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	auto nodeValue = m_interpreter.interpret(GetVariableValueNode(node)).asRvalue();
	memory.put(GetVariableName(node), std::move(nodeValue));
	return "";
}
