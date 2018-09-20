#include "TypeBuilder.h"
#include "TypeBuilderOperator.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "AST.h"

ska::TypeBuilder::TypeBuilder(const SymbolTable& symbolTable) : 
    m_symbols(symbolTable) {
}

bool ska::TypeBuilder::matchVariable(VarTokenEvent& event) const {
    TypeBuilderDispatchCalculation(m_symbols, event.node[0]);
    return true;
}

bool ska::TypeBuilder::matchExpression(ExpressionTokenEvent& event) const {
	TypeBuilderDispatchCalculation(m_symbols, event.node);
    return true;
}

bool ska::TypeBuilder::matchFunction(FunctionTokenEvent& event) const {
    switch(event.type) {
        default: break;
        
        case FunctionTokenEventType::DECLARATION_PARAMETERS: {			
		    TypeBuilderDispatchCalculation(m_symbols, event.node);
        } break;

    }
    return true;
}


