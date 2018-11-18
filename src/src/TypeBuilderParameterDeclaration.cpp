#include "TypeBuilderParameterDeclaration.h"

#include "ExpressionType.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "AST.h"
#include "SymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>::build(const SymbolTable& symbols, const ASTNode& node) {
    assert(node.size() == 1);
    const auto typeStr = node[0].asString();

    if (ExpressionTypeMap.find(typeStr) == ExpressionTypeMap.end()) {
        const auto symbolType = symbols[typeStr];
        if (symbolType == nullptr) {
            throw std::runtime_error("unknown type detected as function parameter : " + node[0].asString());
        }
        SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type built for node \"" << node.asString() << "\" = \"" << symbolType->getType().asString() << "\"";
        return symbolType->getType();
   } else { 
       SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type calculating for node \"" << node.asString() << " with type-node " << typeStr;
       const auto type = ExpressionTypeMap.at(node[0].asString());
       SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type built for node \"" << node.asString() << "\" = \"" << ExpressionTypeSTR[static_cast<std::size_t>(type)] << "\""; 
       return type;
   }
}
