#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorBlock.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::BLOCK>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	memory.createNested();
	auto output = NodeCell{};
	for (auto& child : node) {
		output = m_interpreter.interpret(*child);		
	}
	memory.endNested();
	return output;
}