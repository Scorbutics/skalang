#include "TypeBuilderFunctionPrototypeDeclaration.h"
#include "Config/LoggerConfigLang.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>::build(const Script& script, OperateOn node) {
	auto functionName = node.GetFunctionName();
    auto& symbols = script.symbols();
	const auto* symbolFunction = symbols[functionName];
	auto functionType = Type::MakeCustom<ExpressionType::FUNCTION>(symbolFunction);
	std::size_t index = 0;
    for (auto& paramNode : node) {
		if (index == node.GetParameterSize()) {
			//Handles return value object special case (refers to itself)
			auto type = paramNode->type().value() == ExpressionType::OBJECT && !paramNode->type()->hasSymbol() ?
				Type::MakeCustom<ExpressionType::OBJECT>(symbolFunction) :
				paramNode->type().value();
			functionType.add(type);
		} else {
			auto type = paramNode->type().value();
			functionType.add(type);
		}
		index++;
    }
	
    SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>) << "function prototype declaration \""<< node <<"\" with type "<< functionType;

    return functionType;
}
