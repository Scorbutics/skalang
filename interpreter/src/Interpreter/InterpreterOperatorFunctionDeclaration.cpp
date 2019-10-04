#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "InterpreterOperatorFunctionDeclaration.h"
#include "Interpreter/Value/Script.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::FUNCTION_DECLARATION>::interpret(OperateOn node) {
	assert(!node.GetFunctionName().empty());
	const auto inScriptFunctionIndex = node.parent.pushFunction(node.GetFunction());
	const auto scriptId = node.parent.id();
	auto functionLValue = node.parent.putMemory(node.GetFunctionName(), ScriptVariableRef{ inScriptFunctionIndex, scriptId } );
	return NodeLValue{std::move(functionLValue.first), std::move(functionLValue.second)};
}
