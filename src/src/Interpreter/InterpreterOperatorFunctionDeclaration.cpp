#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorFunctionDeclaration.h"
#include "Service/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FUNCTION_DECLARATION>::interpret(OperateOn node) {
	assert(!node.GetFunctionName().empty());
	auto functionLValue = node.parent.putMemory(node.GetFunctionName(), ExecutionContext{ node.parent, node.GetFunction() });
	return NodeLValue{std::move(functionLValue.first), std::move(functionLValue.second)};
}
