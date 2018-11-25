#include "TypeBuilderParameterDeclaration.h"

#include "ExpressionType.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "AST.h"
#include "SymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>)
SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::PARAMETER_PACK_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>::build(const SymbolTable& symbols, const ASTNode& node) {
    assert(node.size() == 1);
    const auto typeStr = node[0].name();

    if (ExpressionTypeMap.find(typeStr) == ExpressionTypeMap.end()) {
		//Object case
        const auto symbolType = symbols[typeStr];
        if (symbolType == nullptr) {
            throw std::runtime_error("unknown type detected as function parameter : " + node[0].name());
        }
        SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type built for node \"" << node << "\" = \"" << symbolType->getType() << "\"";
		return Type{ typeStr, ExpressionType::OBJECT, *symbolType->symbolTable() };
   } else { 
	   //Built-in case
       SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type calculating for node \"" << node << " with type-node " << typeStr;
       const auto type = ExpressionTypeMap.at(node[0].name());
       SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type built for node \"" << node << "\" = \"" << ExpressionTypeSTR[static_cast<std::size_t>(type)] << "\""; 
	   return Type{ type };
   }
}

