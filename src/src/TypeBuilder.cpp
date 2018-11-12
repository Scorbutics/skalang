#include "LoggerConfigLang.h"
#include "TypeBuilder.h"
#include "TypeBuilderOperator.h"
#include "AST.h"

#include "Parser.h"

SKA_LOGC_CONFIG(LogLevel::Debug, TypeBuilder)

ska::TypeBuilder::TypeBuilder(Parser& parser, const SymbolTable& symbolTable) : 
    m_symbols(symbolTable),
    SubObserver<ExpressionTokenEvent>(std::bind(&TypeBuilder::matchExpression, this, std::placeholders::_1), parser),
	SubObserver<FunctionTokenEvent>(std::bind(&TypeBuilder::matchFunction, this, std::placeholders::_1), parser),
	SubObserver<VarTokenEvent>(std::bind(&TypeBuilder::matchVariable, this, std::placeholders::_1), parser),
	SubObserver<ReturnTokenEvent>(std::bind(&TypeBuilder::matchReturn, this, std::placeholders::_1), parser) {
}

bool ska::TypeBuilder::matchVariable(VarTokenEvent& event) const {
    event.rootNode().buildType(m_symbols);
	SLOG(LogLevel::Debug) << "Type built for variable \"" << event.rootNode().asString() << "\" = \"" << event.rootNode().type().value().asString() << "\"";

    return true;
}

bool ska::TypeBuilder::matchReturn(ReturnTokenEvent& event) const {
	event.rootNode().buildType(m_symbols);
	return true;
}

bool ska::TypeBuilder::matchExpression(ExpressionTokenEvent& event) const {
	event.rootNode().buildType(m_symbols);
	SLOG(LogLevel::Debug) << "Type built for expression \"" << event.rootNode().asString() << "\" = \"" << event.rootNode().type().value().asString() << "\"";
    return true;
}

bool ska::TypeBuilder::matchFunction(FunctionTokenEvent& event) const {
    switch(event.type()) {
        default: break;
		
		case FunctionTokenEventType::CALL:
        case FunctionTokenEventType::DECLARATION_PARAMETERS: {			
			auto& mainFunctionNode = event.rootNode();
			for (auto& functionParametersNode : mainFunctionNode) {
                functionParametersNode->buildType(m_symbols);
			}
			mainFunctionNode.buildType(m_symbols);
			SLOG(LogLevel::Debug) << "Type built for function parameter declaration / call \"" << event.rootNode().asString() << "\" = \"" << event.rootNode().type().value().asString() << "\"";
        } break;

    }
    return true;
}


