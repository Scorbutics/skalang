#include <iostream>

#include "Interpreter.h"

#include "NodeValue/Operator.h"
#include "NodeValue/AST.h"

#include "InterpreterOperatorBinary.h"
#include "InterpreterOperatorBlock.h"
#include "InterpreterOperatorUnary.h"
#include "InterpreterOperator.h"

std::vector<std::unique_ptr<ska::InterpreterOperatorUnit>> ska::Interpreter::build() {
	auto result = std::vector<std::unique_ptr<ska::InterpreterOperatorUnit>> {};
	static constexpr auto maxOperatorEnumIndex = static_cast<std::size_t>(ska::Operator::UNUSED_Last_Length);
	result.resize(maxOperatorEnumIndex);

	result[static_cast<std::size_t>(ska::Operator::BLOCK)] = (std::make_unique<ska::InterpreterOperator<ska::Operator::BLOCK>>(*this));
	result[static_cast<std::size_t>(ska::Operator::BINARY)] = (std::make_unique<ska::InterpreterOperator<ska::Operator::BINARY>>(*this));
	result[static_cast<std::size_t>(ska::Operator::LITERAL)] = (std::make_unique<ska::InterpreterOperator<ska::Operator::LITERAL>>(*this));
	result[static_cast<std::size_t>(ska::Operator::UNARY)] = (std::make_unique<ska::InterpreterOperator<ska::Operator::UNARY>>(*this));
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
