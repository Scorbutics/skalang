#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorExport.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::EXPORT>::interpret(OperateOn node) {
	m_interpreter.interpret({ node.parent, node.GetVariable() });
	return NodeCell{ "", nullptr};
}
