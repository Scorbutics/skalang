#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorBlock.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::BLOCK>::interpret(OperateOn node) {
	m_memory.createNested();
	auto output = NodeCell{};
	for (auto& child : node) {
		output = m_interpreter.interpret(*child);		
	}
	m_memory.endNested();
	return output;
}