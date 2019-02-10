#include <iostream>

#include "Interpreter.h"

#include "NodeValue/Operator.h"
#include "NodeValue/AST.h"

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

	InterpreterOperatorDeclare<ska::Operator::ARRAY_USE>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::ARRAY_DECLARATION>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::IF>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::BRIDGE>(*this, result, m_symbols, m_memory);
    InterpreterOperatorDeclare<ska::Operator::IF_ELSE>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::FUNCTION_DECLARATION>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::FUNCTION_CALL>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::FOR_LOOP>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::FIELD_ACCESS>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::RETURN>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::USER_DEFINED_OBJECT>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::VARIABLE_DECLARATION>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::VARIABLE_AFFECTATION>(*this, result, m_symbols, m_memory);
    InterpreterOperatorDeclare<ska::Operator::BLOCK>(*this, result, m_symbols, m_memory);
    InterpreterOperatorDeclare<ska::Operator::BINARY>(*this, result, m_symbols, m_memory);
    InterpreterOperatorDeclare<ska::Operator::LITERAL>(*this, result, m_symbols, m_memory);
    InterpreterOperatorDeclare<ska::Operator::UNARY>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::IMPORT>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::EXPORT>(*this, result, m_symbols, m_memory);
	return result;
}

ska::Interpreter::Interpreter(SymbolTable& symbols, const ReservedKeywordsPool& reserved) :
	m_symbols(symbols),
	m_operatorInterpreter(build()) /*,
	m_binding(*this, symbols, reserved) */ {
}

ska::NodeCell ska::Interpreter::interpret(ASTNode& node) {
	auto& builder = m_operatorInterpreter[static_cast<std::size_t>(node.op())];
	assert(builder != nullptr);
	return builder->interpret(node);
}

ska::NodeValue ska::Interpreter::script(ASTNode& node) {
	auto value = interpret(node).asRvalue();
	m_memory.clear();
	return value;
}
