#include <fstream>
#include <iostream>

#include "LoggerConfig.h"
#include <Logging/LogAsync.h>
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

std::ofstream TypeBuilderLogFileOutput {"TypeBuilderLogError.txt" };

ska::Logger<ska::TypeBuilder, ska::LogAsync> TypeBuilderLogger {std::cout, [](const ska::LogEntry& entry) {
    return entry.getLogLevel() == ska::LogLevel::Warn;
}};

ska::TypeBuilder::TypeBuilder(Parser& parser, const SymbolTable& symbolTable) : 
    m_symbols(symbolTable),
    SubObserver<ExpressionTokenEvent>(std::bind(&TypeBuilder::matchExpression, this, std::placeholders::_1), parser) {
	static auto done = false;
	if (!done) {
		TypeBuilderLogger.addOutputTarget(TypeBuilderLogFileOutput, [](const ska::LogEntry& entry) {
			return entry.getLogLevel() == ska::LogLevel::Error;
		});
		TypeBuilderLogger.setPattern(LogLevel::Error, "%10c[%h:%m:%s:%T]%12c[Error] %8c(%i) %14c%C %15c%v");
		TypeBuilderLogger.setPattern(LogLevel::Warn, "%10c[%h:%m:%s:%T]%12c[Warn] %8c(%i) %14c%C %15c%v");
		done = true;
	}
}

bool ska::TypeBuilder::matchVariable(VarTokenEvent& event) const {
    TypeBuilderLogger.log<LogLevel::Info>() << "Building type for variable " << event.node.asString();
    TypeBuilderDispatchCalculation(m_symbols, event.node[0]);
    TypeBuilderLogger.log<LogLevel::Debug>() << "Type built " << event.node[0].type.value().asString();

    return true;
}

bool ska::TypeBuilder::matchExpression(ExpressionTokenEvent& event) const {
    TypeBuilderLogger.log<LogLevel::Warn>() << "Building type for expression " << event.node.asString();
	TypeBuilderDispatchCalculation(m_symbols, event.node);
    TypeBuilderLogger.log<LogLevel::Error>() << "Type built " << event.node.type.value().asString();
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


