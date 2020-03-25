#include "TypeBuilderFunctionPrototypeDeclaration.h"
#include "Config/LoggerConfigLang.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "Service/SymbolTable.h"
#include "NodeValue/AST.h"
#include "NodeValue/ScriptAST.h"

#include "Operation/OperationTypeType.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>)
#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>)
#define LOG_WARN SLOG_STATIC(ska::LogLevel::Warn, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>)

namespace ska {
	TypeHierarchy TypeBuilderFunctionPrototypeDeclarationDeduceReturnType(const ASTNode& node, const SymbolTable& symbolTable, const Symbol* symbolFunction) {
		const auto type = node.type().value();
		const auto objectIsVar = type == ExpressionType::OBJECT;
		if (objectIsVar) {
			LOG_INFO << "function prototype declaration has an OBJECT return type \""<< (symbolFunction != nullptr ? symbolFunction->name() : "") << "\"";
			const auto objectDoesntRefersToItself = node.typeSymbol() != nullptr;
			if (!objectDoesntRefersToItself) {
				LOG_WARN << "Linking current function return type to master symbol \"" << (symbolFunction != nullptr ? symbolFunction->name() : "") << "\"";
				return { Type::MakeCustom<ExpressionType::OBJECT>(symbolFunction), node.symbol() == nullptr ? symbolFunction : node.symbol() };
			}
			
			const auto symbolType = node.typeSymbol() == nullptr ? std::optional<Type>{} : node.typeSymbol()->type();
			if (!symbolType.has_value()) {
				throw std::runtime_error("unable to find type symbol type of function");
			}

			LOG_INFO << "function symbol type deduced from type \""<<  symbolType.value() << "\"";
			if (symbolType.value() == ExpressionType::VOID) {
				assert(node.symbol() != nullptr);
				LOG_WARN << "Linking current function return type to master symbol \"" << (symbolFunction != nullptr ? symbolFunction->name() : "") << "\"";
				return { Type::MakeCustom<ExpressionType::OBJECT>(symbolFunction), node.symbol() == nullptr ? symbolFunction : node.symbol() };
			}
			assert(!symbolType.value().empty());
			return { symbolType.value().back(), node.typeSymbol() };
		}
		
		if (type == ExpressionType::FUNCTION) {
			LOG_INFO << "function prototype declaration has a FUNCTION return type \""<< (symbolFunction != nullptr ? symbolFunction->name() : "") << "\"";
			assert(!type.empty());
			return type.back();
		}

		LOG_INFO << "function prototype declaration has a return type \"" << type << "\" for function \"" << (symbolFunction != nullptr ? symbolFunction->name() : "") << "\"";
		return type;
	}
}

ska::TypeHierarchy ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>::build(const ScriptAST& script, OperateOn node) {
	auto functionName = node.GetFunctionName();
    auto& symbols = script.symbols();
	auto* symbolFunction = symbols[functionName];
	const Symbol* returnSymbol = nullptr;
	auto functionType = Type::MakeCustom<ExpressionType::FUNCTION>(nullptr);
	std::size_t index = 0;
    for (auto& paramNode : node) {
			LOG_INFO << paramNode->name();
		if (index == node.GetParameterSize()) {
			auto hierarchy = TypeBuilderFunctionPrototypeDeclarationDeduceReturnType(*paramNode, script.symbols(), symbolFunction);
			returnSymbol = hierarchy.link();
			functionType.add(std::move(hierarchy.type));
		} else {
			auto type = paramNode->type().value();
			functionType.add(std::move(type));
		}
		index++;
    }

		LOG_DEBUG << "function prototype declaration \""<< node.GetFunctionName() <<"\" with type "<< functionType;
	return { functionType, returnSymbol };
}
