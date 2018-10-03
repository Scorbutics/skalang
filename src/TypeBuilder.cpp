#include "LoggerConfig.h"
#include "TypeBuilder.h"
#include "TypeBuilderOperator.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "AST.h"

#include "Parser.h"

#define SKALANG_LOG_TYPE_BUILDER

ska::TypeBuilder::TypeBuilder(Parser& parser, const SymbolTable& symbolTable) : 
    m_symbols(symbolTable),
    SubObserver<ExpressionTokenEvent>(std::bind(&TypeBuilder::matchExpression, this, std::placeholders::_1), parser) {
}

bool ska::TypeBuilder::matchVariable(VarTokenEvent& event) const {
    SKA_LOG_INFO("Building type for variable ", event.node.asString());
    TypeBuilderDispatchCalculation(m_symbols, event.node[0]);
    SKA_LOG_INFO("Type built ", event.node[0].type.value().asString());

    return true;
}

bool ska::TypeBuilder::matchExpression(ExpressionTokenEvent& event) const {
    SKA_LOG_INFO("Building type for expression ", event.node.asString());
	TypeBuilderDispatchCalculation(m_symbols, event.node);
    SKA_LOG_INFO("Type built ", event.node.type.value().asString());
    return true;
}

bool ska::TypeBuilder::matchFunction(FunctionTokenEvent& event) const {
    switch(event.type) {
        default: break;
        
        case FunctionTokenEventType::DECLARATION_PARAMETERS: {			
            SKA_LOG_INFO("Building type for parameter declaration ", event.node.asString()); 
            TypeBuilderDispatchCalculation(m_symbols, event.node);
            SKA_LOG_INFO("Type built ", event.node.type.value().asString());
        } break;

    }
    return true;
}


