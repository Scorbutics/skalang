#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorFunctionDeclaration.h"
#include "Service/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FUNCTION_DECLARATION>::interpret(OperateOn node) {
	assert(!node.GetFunctionName().empty());
	auto functionLValue = node.parent.memory().put(node.GetFunctionName(), ExecutionContext{ node.parent, node.GetFunction() });
	return NodeCell{ functionLValue };
}
