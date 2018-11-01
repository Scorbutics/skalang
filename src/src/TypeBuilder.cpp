#include <fstream>
#include <iostream>

#include "LoggerConfigLang.h"
#include "TypeBuilder.h"
#include "TypeBuilderOperator.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "AST.h"

#include "Parser.h"

SKA_LOGC_CONFIG(LogLevel::Disabled, TypeBuilder)

ska::TypeBuilder::TypeBuilder(Parser& parser, const SymbolTable& symbolTable) : 
    m_symbols(symbolTable),
    SubObserver<ExpressionTokenEvent>(std::bind(&TypeBuilder::matchExpression, this, std::placeholders::_1), parser),
	SubObserver<FunctionTokenEvent>(std::bind(&TypeBuilder::matchFunction, this, std::placeholders::_1), parser),
	SubObserver<VarTokenEvent>(std::bind(&TypeBuilder::matchVariable, this, std::placeholders::_1), parser)  {
}

bool ska::TypeBuilder::matchVariable(VarTokenEvent& event) const {
	SLOG(LogLevel::Info) << "Building type for variable " << event.rootNode().asString();
    TypeBuilderDispatchCalculation(m_symbols, event.rootNode()[0]);
	SLOG(LogLevel::Info) << "Type built " << event.rootNode()[0].type().value().asString();

    return true;
}

bool ska::TypeBuilder::matchExpression(ExpressionTokenEvent& event) const {
	SLOG(LogLevel::Info) << "Building type for expression " << event.rootNode().asString();
	TypeBuilderDispatchCalculation(m_symbols, event.rootNode());
	SLOG(LogLevel::Info) << "Type built " << event.rootNode().type().value().asString();
    return true;
}

bool ska::TypeBuilder::matchFunction(FunctionTokenEvent& event) const {
    switch(event.type()) {
        default: break;
        
        case FunctionTokenEventType::DECLARATION_PARAMETERS: {			
			SLOG(LogLevel::Info) << "Building type for parameter declaration " << event.rootNode().asString();
            TypeBuilderDispatchCalculation(m_symbols, event.rootNode());
			SLOG(LogLevel::Info) << "Type built " << event.rootNode().type().value().asString();
        } break;

    }
    return true;
}


