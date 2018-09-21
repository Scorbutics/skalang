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
#ifdef SKALANG_LOG_TYPE_BUILDER
    std::cout << "[TypeBuilder] Building type for variable " << event.node.asString() << std::endl; 
#endif
    TypeBuilderDispatchCalculation(m_symbols, event.node[0]);
#ifdef SKALANG_LOG_TYPE_BUILDER
    std::cout << "[TypeBuilder] Type built " << event.node[0].type.value().asString() << std::endl;
#endif
    return true;
}

bool ska::TypeBuilder::matchExpression(ExpressionTokenEvent& event) const {
#ifdef SKALANG_LOG_TYPE_BUILDER
    std::cout << "[TypeBuilder] Building type for expression " << event.node.asString() << std::endl; 
#endif
	TypeBuilderDispatchCalculation(m_symbols, event.node);
#ifdef SKALANG_LOG_TYPE_BUILDER
    std::cout << "[TypeBuilder] Type built " << event.node.type.value().asString() << std::endl;
#endif
    return true;
}

bool ska::TypeBuilder::matchFunction(FunctionTokenEvent& event) const {
    switch(event.type) {
        default: break;
        
        case FunctionTokenEventType::DECLARATION_PARAMETERS: {			
#ifdef SKALANG_LOG_TYPE_BUILDER
            std::cout << "[TypeBuilder] Building type for parameter declaration " << event.node.asString() << std::endl; 
#endif	    
            TypeBuilderDispatchCalculation(m_symbols, event.node);
#ifdef SKALANG_LOG_TYPE_BUILDER
            std::cout << "[TypeBuilder] Type built " << event.node.type.value().asString() << std::endl;
#endif
        } break;

    }
    return true;
}


