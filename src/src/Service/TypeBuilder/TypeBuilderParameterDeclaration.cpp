#include "TypeBuilderParameterDeclaration.h"

#include "NodeValue/ExpressionType.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "Service/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>::build(const Script& script, OperateOn node) {
    
    const auto typeStr = node.GetTypeValueNode().name();

	auto type = Type{};
    if (ExpressionTypeMap.find(typeStr) == ExpressionTypeMap.end()) {
		//Object case
        auto& symbols = script.symbols();
        const auto symbolType = symbols[typeStr];
        if (symbolType == nullptr) {
            throw std::runtime_error("unknown type detected as function parameter : " + node.GetTypeValueNode().name());
        }
        SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type built for node \"" << node.GetVariableName() << "\" = \"" << symbolType->getType() << "\"";
		type = Type::MakeCustom<ExpressionType::OBJECT>(symbolType);
   } else { 
	   //Built-in case
       SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type calculating for node \"" << node.GetVariableName() << " with type-node " << typeStr;
	   type = ExpressionTypeMap.at(node.GetTypeValueNode().name());
       SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type built for node \"" << node.GetVariableName() << "\" = \"" << type << "\""; 
   }

	//handles arrays
	if (node.GetTypeValueNode().size() == 1) {
		type = Type::MakeBuiltIn<ExpressionType::ARRAY>().add(type);
	}

	return type;
}

