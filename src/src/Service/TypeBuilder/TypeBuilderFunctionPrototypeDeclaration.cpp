#include "TypeBuilderFunctionPrototypeDeclaration.h"
#include "Config/LoggerConfigLang.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>)

namespace ska {
	Type TypeBuilderFunctionPrototypeDeclarationDeduceReturnType(const ASTNode& node, const Symbol* symbolFunction) {
		const auto type = node.type().value();
		const auto objectIsVar = type == ExpressionType::OBJECT;
		if (objectIsVar) {
			SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>) << "function prototype declaration has an OBJECT return type \""<< (symbolFunction != nullptr ? symbolFunction->getName() : "") << "\"";
			const auto objectDoesntRefersToItself = type.hasSymbol();
			const auto* returnSymbol = objectDoesntRefersToItself ? (node.size() == 1 ? (type)[node[0].name()] : type[node.name()]) : symbolFunction;
			return Type::MakeCustom<ExpressionType::OBJECT>(returnSymbol);
		}
		
		if(type == ExpressionType::FUNCTION) {
			SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>) << "function prototype declaration has a FUNCTION return type \""<< (symbolFunction != nullptr ? symbolFunction->getName() : "") << "\"";
			assert(!type.compound().empty());
			return type.compound().back();
		}

		SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>) << "function prototype declaration has a return type \"" << type << "\" for function \"" << (symbolFunction != nullptr ? symbolFunction->getName() : "") << "\"";
		return type;
	}
}

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>::build(const Script& script, OperateOn node) {
	auto functionName = node.GetFunctionName();
    auto& symbols = script.symbols();
	const auto* symbolFunction = symbols[functionName];
	auto functionType = Type::MakeCustom<ExpressionType::FUNCTION>(symbolFunction);
	std::size_t index = 0;
    for (auto& paramNode : node) {
			SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>) << paramNode->name();
		if (index == node.GetParameterSize()) {
			functionType.add(TypeBuilderFunctionPrototypeDeclarationDeduceReturnType(*paramNode, symbolFunction));
		} else {
			auto type = paramNode->type().value();
			functionType.add(type);
		}
		index++;
    }
	
    //SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>) << "function prototype declaration \""<< node <<"\" with type "<< functionType;

    return functionType;
}
