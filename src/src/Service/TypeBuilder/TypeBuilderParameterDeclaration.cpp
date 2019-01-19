#include "TypeBuilderParameterDeclaration.h"

#include "NodeValue/ExpressionType.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>)
SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::FUNCTION_PROTOTYPE_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>::build(Parser& parser, const SymbolTable& symbols, const ASTNode& node) {
    assert(node.size() == 1);
    const auto typeStr = node[0].name();

	auto type = Type{};
    if (ExpressionTypeMap.find(typeStr) == ExpressionTypeMap.end()) {
		//Object case
        const auto symbolType = symbols[typeStr];
        if (symbolType == nullptr) {
            throw std::runtime_error("unknown type detected as function parameter : " + node[0].name());
        }
        SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type built for node \"" << node << "\" = \"" << symbolType->getType() << "\"";
		type = Type{ typeStr, ExpressionType::OBJECT, *symbolType->symbolTable() };

   } else { 
	   //Built-in case
       SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type calculating for node \"" << node << " with type-node " << typeStr;
	   type = ExpressionTypeMap.at(node[0].name());
       SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type built for node \"" << node << "\" = \"" << type << "\""; 
   }

	//handles arrays
	if (node[0].size() == 1) {
		type = Type{ ExpressionType::ARRAY }.add(type);
	}

	return type;
}

