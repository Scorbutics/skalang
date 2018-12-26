#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorArrayUse.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::ARRAY_USE>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	auto& arrayCell = *std::get<std::shared_ptr<std::vector<NodeValue>>>(*memory[node[0].name()]);
	auto arrayIndex = nodeval<int>(m_interpreter.interpret(node[1]).asRvalue());
	if (arrayCell.size() <= arrayIndex) {
		throw std::runtime_error("array index out of bounds");
	}
	return &arrayCell[arrayIndex];
}
