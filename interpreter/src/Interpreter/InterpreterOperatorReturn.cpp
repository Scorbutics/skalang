#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorReturn.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::RETURN>::interpret(OperateOn node) {
	return m_interpreter.interpret({ node.parent, node.GetValue() });
}
