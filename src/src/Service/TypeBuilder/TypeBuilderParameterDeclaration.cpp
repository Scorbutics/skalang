#include "TypeBuilderParameterDeclaration.h"

#include "NodeValue/ExpressionType.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "NodeValue/AST.h"
#include "Service/SymbolTable.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>)

ska::Type ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>::build(const Script& script, OperateOn node) {
    
    const auto& type = node.GetTypeValueNode();
	auto result = Type{};
    if (type.size() == 1 || ExpressionTypeMap.find(type.name()) == ExpressionTypeMap.end()) {
		//Object case		
		const Symbol* symbolType = nullptr;
		
		auto& symbols = script.symbols();
		if (type.size() == 1) {
			const auto* importedScriptSymbol = symbols[type.name()];
			symbolType = importedScriptSymbol == nullptr ? nullptr : (importedScriptSymbol->getType())[type[0].name()];
		} else {
			symbolType = symbols[type.name()];
		}

		SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type built for node \"" << node.GetVariableName() << "\" = \"" << (symbolType != nullptr ? symbolType->getType() : Type{}) << "\"";
		if (symbolType == nullptr) {
			throw std::runtime_error("undeclared custom type \"" + (type.size() == 1 ? type.name() + "." + type[0].name() : type.name()) + "\"");
		}
		
		result = Type::MakeCustom<ExpressionType::OBJECT>(symbolType);	
   } else { 
	   //Built-in case
       SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type calculating for node \"" << node.GetVariableName() << " with type-node " << type.name();
	   result = ExpressionTypeMap.at(node.GetTypeValueNode().name());

	   auto& symbols = script.symbols();
	   const auto symbolType = symbols[node.GetVariableName()];
	   if (symbolType != nullptr) {
		   result = Type::MakeBuiltIn(result.type(), symbolType);
	   }
       SLOG_STATIC(ska::LogLevel::Info, ska::TypeBuilderOperator<ska::Operator::PARAMETER_DECLARATION>) << "Parameter declaration type built for node \"" << node.GetVariableName() << "\" = \"" << type << "\""; 
   }

	//handles arrays
	if (node.GetTypeArrayNode() != nullptr) {
		result = Type::MakeBuiltIn<ExpressionType::ARRAY>().add(result);
	}

	return result;
}

