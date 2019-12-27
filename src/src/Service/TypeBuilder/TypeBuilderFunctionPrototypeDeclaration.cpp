#include "TypeBuilderFunctionPrototypeDeclaration.h"
#include "Config/LoggerConfigLang.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"
#include "NodeValue/ScriptAST.h"

#include "Operation/OperationTypeType.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>)

namespace ska {
	Type TypeBuilderFunctionPrototypeDeclarationDeduceReturnType(const ASTNode& node, const SymbolTable& symbolTable, const Symbol* symbolFunction) {
		const auto type = node.type().value();
		const auto objectIsVar = type == ExpressionType::OBJECT;
		if (objectIsVar) {
			SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>) << "function prototype declaration has an OBJECT return type \""<< (symbolFunction != nullptr ? symbolFunction->getName() : "") << "\"";
			const auto objectDoesntRefersToItself = type.hasSymbol();
			if (!objectDoesntRefersToItself) {
				return Type::MakeCustom<ExpressionType::OBJECT>(symbolFunction);
			}
			const auto operateOnType = OperationType<Operator::TYPE>{node};
			const auto* symbol = operateOnType.GetSymbol(symbolTable);
			assert(symbol != nullptr);
			SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>) << "function symbol type deduced from type \""<<  symbol->getType() << "\"";
			if(symbol->getType() == ExpressionType::VOID) {
				return Type::MakeCustom<ExpressionType::OBJECT>(symbolFunction);
			}
			assert(!symbol->getType().compound().empty());
			return symbol->getType().compound().back();

			/*
			const auto* returnSymbol = objectDoesntRefersToItself ? (node.size() == 1 ? (type)[node[0].name()] : type[node.name()]) : symbolFunction;
			return Type::MakeCustom<ExpressionType::OBJECT>(returnSymbol);
			*/
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

ska::Type ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
	auto functionName = node.GetFunctionName();
    auto& symbols = script.symbols();
	const auto* symbolFunction = symbols[functionName];
	auto functionType = Type::MakeCustom<ExpressionType::FUNCTION>(symbolFunction);
	std::size_t index = 0;
    for (auto& paramNode : node) {
			SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>) << paramNode->name();
		if (index == node.GetParameterSize()) {
			functionType.add(TypeBuilderFunctionPrototypeDeclarationDeduceReturnType(*paramNode, script.symbols(), symbolFunction));
		} else {
			auto type = paramNode->type().value();
			functionType.add(type);
		}
		index++;
    }
	
    //SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>) << "function prototype declaration \""<< node <<"\" with type "<< functionType;

    return functionType;
}
