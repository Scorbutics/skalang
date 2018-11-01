#include <iostream>
#include "LoggerConfig.h"
#include "Parser.h"
#include "SemanticTypeChecker.h"
#include "SymbolTable.h"
#include "AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::SemanticTypeChecker)

ska::SemanticTypeChecker::SemanticTypeChecker(Parser& parser) :
    SubObserver<VarTokenEvent>(std::bind(&SemanticTypeChecker::matchVariable, this, std::placeholders::_1), parser),
    SubObserver<FunctionTokenEvent>(std::bind(&SemanticTypeChecker::matchFunction, this, std::placeholders::_1), parser) {
}

bool ska::SemanticTypeChecker::matchFunction(FunctionTokenEvent& token) {
    const auto variable = token.node[0].asString();
 
    switch(token.type) {
        case FunctionTokenEventType::DECLARATION_STATEMENT:break;
        
        case FunctionTokenEventType::DECLARATION_PARAMETERS: {			
			//getExpressionType(token.node);
			const auto returnType = token.node[token.node.size() - 1].type;
            if (returnType.has_value() && returnType.value() == ExpressionType::OBJECT) {
				SLOG(ska::LogLevel::Debug) << "user defined object type detected";
			}

        } break;

        default:
        case FunctionTokenEventType::CALL: {
			//auto type = getExpressionType(token.node);
            const auto symbol = (*m_symbols)[variable];
            if(symbol == nullptr || symbol->getType() != ExpressionType::FUNCTION) {
                throw std::runtime_error("function " + variable + " is called before being declared (or has a bad declaration)");
            }

            if(symbol->size() != token.node.size()) {
                auto ss = std::stringstream {};
                ss << "bad function call : the function " << variable << " needs " << (symbol->size() - 1) << " parameters but is being called with " << (token.node.size() - 1) << " parameters (function type is " << symbol->getType().asString() << ")";
                throw std::runtime_error(ss.str());
            }

			SLOG(ska::LogLevel::Debug) << variable << " function has the following arguments types during its call : ";

            for(auto index = 1u; index < token.node.size(); index++) {
                auto& param = token.node[index];
                const auto calculatedType = param.type.value();
				SLOG(ska::LogLevel::Debug) << index << " \"" << calculatedType.asString() << "\" while a type convertible to \"" << (*symbol)[index].asString() << "\" is required";

                if((*symbol)[index - 1].crossTypes('=', calculatedType) == ExpressionType::VOID) {
                    auto ss = std::stringstream {};
                    ss << "Type  \"" << calculatedType.asString() << "\" is encountered while a type convertible to \"" << (*symbol)[index].asString() << "\" is required";
                    throw std::runtime_error(ss.str());
                }
            }
        } break;
    }
    
    return true;
}

bool ska::SemanticTypeChecker::matchVariable(VarTokenEvent& token) {
    assert((token.node.size() >= 1 && token.node[0].type.has_value()) && "The current variable has no type. Maybe you forgot to add a type builder ?");
    const auto tokenNodeExpressionType = token.node[0].type.value();
	const auto value = token.node[0].asString();
    const auto variable = token.node.asString();
    const auto symbol = (*m_symbols)[variable];
    
	SLOG(ska::LogLevel::Debug) << variable << " = " << value << ";\tsymbol = " << tokenNodeExpressionType.asString();

    if(symbol != nullptr && token.type == VarTokenEventType::AFFECTATION) {
        const auto newTokenType = symbol->getType().crossTypes('=', tokenNodeExpressionType);
        if(newTokenType == ExpressionType::VOID) {
            const auto expressionTypeIndex = tokenNodeExpressionType;
            throw std::runtime_error("The symbol \"" + variable + "\" has already been declared as " + 
                std::string(symbol->getType().asString()) + " but is now wanted to be " +
                std::string(expressionTypeIndex.asString()));		
        }
    }

    return true;
}

void ska::SemanticTypeChecker::setSymbolTable(const SymbolTable& symbolTable) {
    m_symbols = &symbolTable;
}

