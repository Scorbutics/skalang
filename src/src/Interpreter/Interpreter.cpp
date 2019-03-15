#include <iostream>

#include "Interpreter.h"

#include "NodeValue/Operator.h"
#include "NodeValue/AST.h"
#include "Service/Script.h"

#include "InterpreterOperatorBinary.h"
#include "InterpreterOperatorIfElse.h"
#include "InterpreterOperatorFunctionCall.h"
#include "InterpreterOperatorFunctionDeclaration.h"
#include "InterpreterOperatorArrayDeclaration.h"
#include "InterpreterOperatorArrayUse.h"
#include "InterpreterOperatorFieldAccess.h"
#include "InterpreterOperatorUserDefinedObject.h"
#include "InterpreterOperatorFor.h"
#include "InterpreterOperatorVariableDeclaration.h"
#include "InterpreterOperatorVariableAffectation.h"
#include "InterpreterOperatorBlock.h"
#include "InterpreterOperatorUnary.h"
#include "InterpreterOperator.h"
#include "InterpreterOperatorImport.h"
#include "InterpreterOperatorExport.h"
#include "InterpreterOperatorBridge.h"
#include "InterpreterOperatorReturn.h"

#include "InterpreterDeclarer.h"

std::vector<std::unique_ptr<ska::InterpreterOperatorUnit>> ska::Interpreter::build() {
	auto result = std::vector<std::unique_ptr<ska::InterpreterOperatorUnit>> {};
	static constexpr auto maxOperatorEnumIndex = static_cast<std::size_t>(ska::Operator::UNUSED_Last_Length);
	result.resize(maxOperatorEnumIndex);

	InterpreterOperatorDeclare<ska::Operator::ARRAY_USE>(*this, result);
	InterpreterOperatorDeclare<ska::Operator::ARRAY_DECLARATION>(*this, result);
	InterpreterOperatorDeclare<ska::Operator::IF>(*this, result);
	InterpreterOperatorDeclare<ska::Operator::BRIDGE>(*this, result);
    InterpreterOperatorDeclare<ska::Operator::IF_ELSE>(*this, result );
	InterpreterOperatorDeclare<ska::Operator::FUNCTION_DECLARATION>(*this, result );
	InterpreterOperatorDeclare<ska::Operator::FUNCTION_CALL>(*this, result );
	InterpreterOperatorDeclare<ska::Operator::FOR_LOOP>(*this, result );
	InterpreterOperatorDeclare<ska::Operator::FIELD_ACCESS>(*this, result );
	InterpreterOperatorDeclare<ska::Operator::RETURN>(*this, result );
	InterpreterOperatorDeclare<ska::Operator::USER_DEFINED_OBJECT>(*this, result );
	InterpreterOperatorDeclare<ska::Operator::VARIABLE_DECLARATION>(*this, result );
	InterpreterOperatorDeclare<ska::Operator::VARIABLE_AFFECTATION>(*this, result );
    InterpreterOperatorDeclare<ska::Operator::BLOCK>(*this, result );
    InterpreterOperatorDeclare<ska::Operator::BINARY>(*this, result );
    InterpreterOperatorDeclare<ska::Operator::LITERAL>(*this, result );
    InterpreterOperatorDeclare<ska::Operator::UNARY>(*this, result );
	InterpreterOperatorDeclare<ska::Operator::IMPORT>(*this, result );
	InterpreterOperatorDeclare<ska::Operator::EXPORT>(*this, result );
	return result;
}

ska::Interpreter::Interpreter(const ReservedKeywordsPool& reserved) :
	m_operatorInterpreter(build()) {
}

void ska::Interpreter::bind(Script& script, const std::string& functionName, BridgeFunctionPtr bridge) {
	script.memory().emplace(functionName, NodeValue{ std::move(bridge) });
}

ska::NodeCell ska::Interpreter::interpret(ExecutionContext node) {
	auto& builder = m_operatorInterpreter[static_cast<std::size_t>(node.pointer().op())];
	assert(builder != nullptr);
	return builder->interpret(node);
}

ska::NodeValue ska::Interpreter::script(Script& script) {
	return interpret(script).asRvalue();
}
