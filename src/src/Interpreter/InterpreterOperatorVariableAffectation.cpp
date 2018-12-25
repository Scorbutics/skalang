#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorVariableAffectation.h"

ska::NodeValue ska::InterpreterOperator<ska::Operator::VARIABLE_AFFECTATION>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	auto nodeValue = m_interpreter.interpret(node[1]);
	memory.put(node[0].name(), nodeValue.clone());
	return nodeValue.clone();
}
