#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorArrayDeclaration.h"

ska::NodeValue ska::InterpreterOperator<ska::Operator::ARRAY_DECLARATION>::interpret(const SymbolTable& symbols, MemoryTable& memory, ASTNode& node) {
	auto result = std::make_shared<std::vector<Token::Variant>>();
	for (auto& child : node) {
		result->push_back(std::get<Token::Variant>(m_interpreter.interpret(*child)));
	}
	return result;
}
