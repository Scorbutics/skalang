#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorFieldAccess.h"
#include "Interpreter/Value/Script.h"
#include "Runtime/Value/ObjectMemory.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FIELD_ACCESS>::interpret(OperateOn node) {
	auto object = m_interpreter.interpret({ node.parent, node.GetObject() });
	return object(node.parent, node.GetFieldName());
}
