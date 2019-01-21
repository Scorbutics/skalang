#include "TypeBuilderParameterDeclaration.h"

#include "NodeValue/ExpressionType.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>::build(const SymbolTable& symbols, OperateOn node) {
    
    const auto typeStr = node.GetVariableValueNode().name();

	auto type = Type{};
    if (ExpressionTypeMap.find(typeStr) == ExpressionTypeMap.end()) {
		//Object case
        const auto symbolType = symbols[typeStr];
        if (symbolType == nullptr) {
            throw std::runtime_error("unknown type detected as function parameter : " + node.GetVariableValueNode().name());
        }
        SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type built for node \"" << node.GetVariableName() << "\" = \"" << symbolType->getType() << "\"";
		type = Type{ typeStr, ExpressionType::OBJECT, *symbolType->symbolTable() };
   } else { 
	   //Built-in case
       SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type calculating for node \"" << node.GetVariableName() << " with type-node " << typeStr;
	   type = ExpressionTypeMap.at(node.GetVariableValueNode().name());
       SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type built for node \"" << node.GetVariableName() << "\" = \"" << type << "\""; 
   }

	//handles arrays
	if (node.GetVariableValueNode().size() == 1) {
		type = Type{ ExpressionType::ARRAY }.add(type);
	}

	return type;
}

