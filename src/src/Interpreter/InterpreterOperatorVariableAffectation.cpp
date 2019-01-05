#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorVariableAffectation.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::VARIABLE_AFFECTATION>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	auto memCell = m_interpreter.interpret(GetVariableNameNode(node));
	memCell.asLvalue() = m_interpreter.interpret(GetVariableValueNode(node)).asRvalue();
	return memCell;
}
