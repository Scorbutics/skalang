#include "LoggerConfig.h"
#include "TypeBuilder.h"
#include "TypeBuilderOperator.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "AST.h"

#include "Parser.h"

#define SKALANG_LOG_TYPE_BUILDER

namespace ska {
    template <>
    class LoggerClassLevel<TypeBuilder> {
    public:
        static constexpr const auto level = LogLevel::Error;
    };
}

ska::Logger<ska::TypeBuilder> TypeBuilderLogger;

ska::TypeBuilder::TypeBuilder(Parser& parser, const SymbolTable& symbolTable) : 
    m_symbols(symbolTable),
    SubObserver<ExpressionTokenEvent>(std::bind(&TypeBuilder::matchExpression, this, std::placeholders::_1), parser) {
}

bool ska::TypeBuilder::matchVariable(VarTokenEvent& event) const {
    TypeBuilderLogger.log<LogLevel::Info>() << "Building type for variable " << event.node.asString();
    TypeBuilderDispatchCalculation(m_symbols, event.node[0]);
    TypeBuilderLogger.log<LogLevel::Info>() << "Type built " << event.node[0].type.value().asString();

    return true;
}

bool ska::TypeBuilder::matchExpression(ExpressionTokenEvent& event) const {
    TypeBuilderLogger.log<LogLevel::Info>() << "Building type for expression " << event.node.asString();
	TypeBuilderDispatchCalculation(m_symbols, event.node);
    TypeBuilderLogger.log<LogLevel::Info>() << "Type built " << event.node.type.value().asString();
    return true;
}

bool ska::TypeBuilder::matchFunction(FunctionTokenEvent& event) const {
    switch(event.type) {
        default: break;
        
        case FunctionTokenEventType::DECLARATION_PARAMETERS: {			
            TypeBuilderLogger.log<LogLevel::Info>() << "Building type for parameter declaration " << event.node.asString(); 
            TypeBuilderDispatchCalculation(m_symbols, event.node);
            TypeBuilderLogger.log<LogLevel::Info>() << "Type built " << event.node.type.value().asString();
        } break;

    }
    return true;
}


