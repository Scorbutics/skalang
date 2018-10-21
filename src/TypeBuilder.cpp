#include <fstream>
#include <iostream>

#include "LoggerConfig.h"
#include "TypeBuilder.h"
#include "TypeBuilderOperator.h"
#include "TypeBuilderCalculatorDispatcher.h"
#include "AST.h"

#include "Parser.h"

namespace ska {
    template <>
    class LoggerClassLevel<TypeBuilder> {
    public:
        static constexpr const auto level = LogLevel::Info;
    };
}

<<<<<<< HEAD
=======
std::ofstream TypeBuilderLogFileOutput {"TypeBuilderLogError.txt" };

ska::MultiLogger<
	ska::Logger<ska::LogLevel::Info, ska::LogLevel::Warn, ska::LogAsync>,
	ska::Logger<ska::LogLevel::Error, ska::LogLevel::Error, ska::LogSync>
> TypeBuilderLogger;


>>>>>>> 0f35142bda004ea19addca75da07e407533b16b5
ska::TypeBuilder::TypeBuilder(Parser& parser, const SymbolTable& symbolTable) : 
    m_symbols(symbolTable),
    SubObserver<ExpressionTokenEvent>(std::bind(&TypeBuilder::matchExpression, this, std::placeholders::_1), parser) {
}

bool ska::TypeBuilder::matchVariable(VarTokenEvent& event) const {
	SLOG(LogLevel::Info) << "Building type for variable " << event.node.asString();
    TypeBuilderDispatchCalculation(m_symbols, event.node[0]);
	SLOG(LogLevel::Debug) << "Type built " << event.node[0].type.value().asString();

    return true;
}

bool ska::TypeBuilder::matchExpression(ExpressionTokenEvent& event) const {
	SLOG(LogLevel::Warn) << "Building type for expression " << event.node.asString();
	TypeBuilderDispatchCalculation(m_symbols, event.node);
	SLOG(LogLevel::Error) << "Type built " << event.node.type.value().asString();
    return true;
}

bool ska::TypeBuilder::matchFunction(FunctionTokenEvent& event) const {
    switch(event.type) {
        default: break;
        
        case FunctionTokenEventType::DECLARATION_PARAMETERS: {			
			SLOG(LogLevel::Info) << "Building type for parameter declaration " << event.node.asString();
            TypeBuilderDispatchCalculation(m_symbols, event.node);
			SLOG(LogLevel::Info) << "Type built " << event.node.type.value().asString();
        } break;

    }
    return true;
}


