#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorBlock.h"

ska::NodeValue ska::InterpreterOperator<ska::Operator::BLOCK>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	memory.createNested();
	for (auto& child : node) {
		m_interpreter.interpret(*child);
	}
	memory.endNested();
	return "";
}