#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorExport.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::EXPORT>::interpret(OperateOn node) {
	return NodeCell {""};
}
