#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorIfElse.h"

ska::NodeValue ska::InterpreterOperator<ska::Operator::IF>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	auto conditionValue = m_interpreter.interpret(node[0]);
	if(nodeval<bool>(conditionValue)) {
        m_interpreter.interpret(node[1]);
    }
	
	return "";
}

ska::NodeValue ska::InterpreterOperator<ska::Operator::IF_ELSE>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	auto conditionValue = m_interpreter.interpret(node[0]);
	if(nodeval<bool>(conditionValue)) {
        m_interpreter.interpret(node[1]);
    } else {
        m_interpreter.interpret(node[2]);
    }
	
	return "";
}
