#include <iostream>

#include "Interpreter.h"

#include "NodeValue/Operator.h"
#include "NodeValue/AST.h"

#include "InterpreterOperatorBinary.h"
#include "InterpreterOperatorVariableDeclaration.h"
#include "InterpreterOperatorVariableAffectation.h"
#include "InterpreterOperatorBlock.h"
#include "InterpreterOperatorUnary.h"
#include "InterpreterOperator.h"

#include "InterpreterDeclarer.h"

std::vector<std::unique_ptr<ska::InterpreterOperatorUnit>> ska::Interpreter::build() {
	auto result = std::vector<std::unique_ptr<ska::InterpreterOperatorUnit>> {};
	static constexpr auto maxOperatorEnumIndex = static_cast<std::size_t>(ska::Operator::UNUSED_Last_Length);
	result.resize(maxOperatorEnumIndex);

	InterpreterOperatorDeclare<ska::Operator::VARIABLE_DECLARATION>(*this, result);
	InterpreterOperatorDeclare<ska::Operator::VARIABLE_AFFECTATION>(*this, result);
    InterpreterOperatorDeclare<ska::Operator::BLOCK>(*this, result);
    InterpreterOperatorDeclare<ska::Operator::BINARY>(*this, result);
    InterpreterOperatorDeclare<ska::Operator::LITERAL>(*this, result);
    InterpreterOperatorDeclare<ska::Operator::UNARY>(*this, result);
	return result;
}

ska::Interpreter::Interpreter(SymbolTable& symbols) :
	m_symbols(symbols),
	m_operatorInterpreter(build()) {
}

void ska::Interpreter::interpret(ASTNodePtr root) {
	interpret(*root);
}

const ska::Token::Variant& ska::Interpreter::interpret(ASTNode& node) {
	auto& builder = m_operatorInterpreter[static_cast<std::size_t>(node.op())];
	assert(builder != nullptr);
	node.buildValue(builder->interpret(m_symbols, m_memory, node));

	std::cout << node.valueAsString() << std::endl;

	return node.value();
}
