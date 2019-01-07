#include "NodeValue/AST.h"
#include "Interpreter.h"
#include "MemoryTable.h"
#include "InterpreterOperatorArrayDeclaration.h"

ska::NodeCell ska::InterpreterOperator<ska::Operator::ARRAY_DECLARATION>::interpret(ASTNode& node) {
	auto result = std::make_shared<std::vector<NodeValue>>();
	for (auto& child : node) {
		result->push_back(std::get<NodeValue>(m_interpreter.interpret(*child)));
	}
	return result;
}
