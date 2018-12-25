#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorVariableAffectation.h"

ska::NodeValue ska::InterpreterOperator<ska::Operator::VARIABLE_AFFECTATION>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	auto nodeValue = m_interpreter.interpret(node[1]);
	switch (node[0].op()) {
	case Operator::ARRAY_USE:
		m_interpreter.interpret(node[0]);
		assert(!node[0][0].name().empty());
		memory.put(node[0][0].name(), nodeval<int>(node[0][1].value()), nodeValue.clone());
		break;
	default:
		assert(!node[0].name().empty());
		memory.put(node[0].name(), nodeValue.clone());
		break;
	}
	
	return nodeValue.clone();
}
