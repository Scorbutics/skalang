#include <iostream>

#include "Interpreter.h"

#include "NodeValue/Operator.h"
#include "NodeValue/AST.h"

#include "InterpreterOperatorBinary.h"
#include "InterpreterOperatorIfElse.h"
#include "InterpreterOperatorArrayDeclaration.h"
#include "InterpreterOperatorArrayUse.h"
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

	InterpreterOperatorDeclare<ska::Operator::ARRAY_USE>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::ARRAY_DECLARATION>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::IF>(*this, result, m_symbols, m_memory);
    InterpreterOperatorDeclare<ska::Operator::IF_ELSE>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::VARIABLE_DECLARATION>(*this, result, m_symbols, m_memory);
	InterpreterOperatorDeclare<ska::Operator::VARIABLE_AFFECTATION>(*this, result, m_symbols, m_memory);
    InterpreterOperatorDeclare<ska::Operator::BLOCK>(*this, result, m_symbols, m_memory);
    InterpreterOperatorDeclare<ska::Operator::BINARY>(*this, result, m_symbols, m_memory);
    InterpreterOperatorDeclare<ska::Operator::LITERAL>(*this, result, m_symbols, m_memory);
    InterpreterOperatorDeclare<ska::Operator::UNARY>(*this, result, m_symbols, m_memory);
	return result;
}

ska::Interpreter::Interpreter(SymbolTable& symbols) :
	m_symbols(symbols),
	m_operatorInterpreter(build()) {
}

ska::NodeCell ska::Interpreter::interpret(ASTNode& node) {
	auto& builder = m_operatorInterpreter[static_cast<std::size_t>(node.op())];
	assert(builder != nullptr);
	auto res = builder->interpret(node);
	//auto val = std::move(std::holds_alternative<NodeValue*>(res) ? std::get<NodeValue*>(res)->clone() : std::get<NodeValue>(res).clone());
	//node.buildValue(val.clone());

	//std::cout << node.name() << " " << node.valueAsString() << std::endl;
	return res;
}
