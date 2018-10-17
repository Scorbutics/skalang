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

ska::Logger<ska::LogLevel::Info, ska::LogLevel::Error, ska::LogAsync> TypeBuilderLoggerFile {TypeBuilderLogFileOutput};
ska::Logger<ska::LogLevel::Error, ska::LogLevel::Error, ska::LogSync> TypeBuilderLoggerConsole {std::cout};

ska::TypeBuilder::TypeBuilder(Parser& parser, const SymbolTable& symbolTable) : 
    m_symbols(symbolTable),
    SubObserver<ExpressionTokenEvent>(std::bind(&TypeBuilder::matchExpression, this, std::placeholders::_1), parser) {
	static auto done = false;
	if (!done) {
		/*TypeBuilderLogger.addOutputTarget(TypeBuilderLogFileOutput, [](const ska::LogEntry& entry) {
			return entry.getLogLevel() == ska::LogLevel::Error;
		});
		TypeBuilderLogger.setPattern(LogLevel::Error, "%10c[%h:%m:%s:%T]%12c[Error] %8c(%i) %14c%C %15c%v");
		TypeBuilderLogger.setPattern(LogLevel::Warn, "%10c[%h:%m:%s:%T]%12c[Warn] %8c(%i) %14c%C %15c%v");*/
		done = true;
	}
}

bool ska::TypeBuilder::matchVariable(VarTokenEvent& event) const {
	SLOG(TypeBuilderLogger, LogLevel::Info, ska::TypeBuilder) << "Building type for variable " << event.node.asString();
    TypeBuilderDispatchCalculation(m_symbols, event.node[0]);
	SLOG(TypeBuilderLogger, LogLevel::Debug, ska::TypeBuilder) << "Type built " << event.node[0].type.value().asString();

    return true;
}

bool ska::TypeBuilder::matchExpression(ExpressionTokenEvent& event) const {
	SLOG(TypeBuilderLogger, LogLevel::Warn, ska::TypeBuilder) << "Building type for expression " << event.node.asString();
	TypeBuilderDispatchCalculation(m_symbols, event.node);
	SLOG(TypeBuilderLogger, LogLevel::Error, ska::TypeBuilder) << "Type built " << event.node.type.value().asString();
    return true;
}

bool ska::TypeBuilder::matchFunction(FunctionTokenEvent& event) const {
    switch(event.type) {
        default: break;
        
        case FunctionTokenEventType::DECLARATION_PARAMETERS: {			
			SLOG(TypeBuilderLogger, LogLevel::Info, ska::TypeBuilder) << "Building type for parameter declaration " << event.node.asString();
            TypeBuilderDispatchCalculation(m_symbols, event.node);
			SLOG(TypeBuilderLogger, LogLevel::Info, ska::TypeBuilder) << "Type built " << event.node.type.value().asString();
        } break;

    }
    return true;
}


