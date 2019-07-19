#include <string>
#include "NodeValue/AST.h"
#include "GeneratorOperatorUnary.h"
#include "Interpreter/Value/Script.h"

namespace ska {
	BytecodeCell GeneratorOperatorConvertValue(ExpressionType type, const TokenVariant& value) {
        auto result = TokenVariant {};
        auto resultType = ExpressionType::STRING;
        switch(type) {
		case ExpressionType::INT:
            result = std::holds_alternative<int>(value) ? value : std::stoi(*std::get<StringShared>(value));
            break;
        case ExpressionType::FLOAT:
            result = std::holds_alternative<double>(value) ? value : std::stof(*std::get<StringShared>(value));
		    break;
        case ExpressionType::BOOLEAN:
			result = std::holds_alternative<bool>(value) ? value : (*std::get<StringShared>(value) == "true");
		    break;
        default:
			result = std::get<StringShared>(value);
            break;
        }

        return { resultType, std::move(result) };
    }
}

ska::BytecodeCell ska::GeneratorOperator<ska::Operator::UNARY>::generate(OperateOn node) {
	assert(!node.GetValue().empty());
	return { node.asNode().type(), node.GetValue() };
}

ska::BytecodeCell ska::GeneratorOperator<ska::Operator::LITERAL>::interpret(OperateOn node) {
	return GeneratorOperatorConvertValue(node.GetType().value().type(), std::make_shared<std::string>(node.GetValue()));
}
