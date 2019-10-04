#include <string>
#include "NodeValue/AST.h"
#include "MemoryTable.h"
#include "InterpreterOperatorUnary.h"
#include "Interpreter/Value/Script.h"

namespace ska {
	NodeValue InterpreterOperatorConvertValue(ExpressionType type, const TokenVariant& value) {
        switch(type) {
		case ExpressionType::INT:
            return std::holds_alternative<int>(value) ? value : std::stoi(*std::get<StringShared>(value));
		case ExpressionType::FLOAT:
            return std::holds_alternative<double>(value) ? value : std::stof(*std::get<StringShared>(value));
		case ExpressionType::BOOLEAN:
			return std::holds_alternative<bool>(value) ? value : (*std::get<StringShared>(value) == "true");
		default:
			return std::get<StringShared>(value);
        }
    }
}

ska::NodeCell ska::InterpreterOperator<ska::Operator::UNARY>::interpret(OperateOn node) {
	assert(!node.GetValue().empty());
	auto nodeValue = node.parent.findInMemoryTree(node.GetValue());
	if(nodeValue.first == nullptr) {
		throw std::runtime_error("unable to find symbol \"" + node.GetValue() + "\" in memory of current script \"" + node.parent.name() + "\"");
	}

	return NodeLValue{ std::move(nodeValue.first), std::move(nodeValue.second) };
}

ska::NodeCell ska::InterpreterOperator<ska::Operator::LITERAL>::interpret(OperateOn node) {
	return NodeRValue{ InterpreterOperatorConvertValue(node.GetType().value().type(), std::make_shared<std::string>(node.GetValue())), nullptr };
}
