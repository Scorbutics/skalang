#include <string>
#include "NodeValue/AST.h"
#include "MemoryTable.h"
#include "InterpreterOperatorUnary.h"
#include "Service/Script.h"

namespace ska {
	NodeValue InterpreterOperatorConvertString(ExpressionType type, const TokenVariant& value) {
        switch(type) {
		case ExpressionType::INT:
            return std::holds_alternative<int>(value) ? value : std::stoi(std::get<std::string>(value));
		case ExpressionType::FLOAT:
            return std::holds_alternative<double>(value) ? value : std::stof(std::get<std::string>(value));
		case ExpressionType::BOOLEAN:
			return std::holds_alternative<bool>(value) ? value : (std::get<std::string>(value) == "true");
		default:
			return std::get<std::string>(value);
        }
    }
}

ska::NodeCell ska::InterpreterOperator<ska::Operator::UNARY>::interpret(OperateOn node) {
	assert(!node.GetValue().empty());
	auto nodeValue = node.parent.findInMemoryTree(node.GetValue());
    if(nodeValue.first == nullptr) {
        return NodeCell {"", nullptr };
    }

	return NodeLValue{ nodeValue.first, nodeValue.second };
}

ska::NodeCell ska::InterpreterOperator<ska::Operator::LITERAL>::interpret(OperateOn node) {
	return NodeCell{ InterpreterOperatorConvertString(node.GetType().value().type(), node.GetValue()), nullptr };
}
