#include <iostream>
#include "LoggerConfigLang.h"
#include "Parser.h"
#include "SemanticTypeChecker.h"
#include "SymbolTable.h"
#include "AST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::SemanticTypeChecker)

ska::SemanticTypeChecker::SemanticTypeChecker(Parser& parser) :
    SubObserver<VarTokenEvent>(std::bind(&SemanticTypeChecker::matchVariable, this, std::placeholders::_1), parser),
    SubObserver<FunctionTokenEvent>(std::bind(&SemanticTypeChecker::matchFunction, this, std::placeholders::_1), parser) {
}

bool ska::SemanticTypeChecker::matchFunction(const FunctionTokenEvent& token) {
    switch(token.type()) {
        case FunctionTokenEventType::DECLARATION_STATEMENT:break;
        
        case FunctionTokenEventType::DECLARATION_PARAMETERS: {			
			const auto returnType = token.contextNode().type();
            if (returnType.has_value() && returnType.value() == ExpressionType::OBJECT) {
				SLOG(ska::LogLevel::Debug) << "user defined object type detected";
			}

        } break;

        default:
        case FunctionTokenEventType::CALL: {
			assert(token.rootNode().size() > 0);
			const auto& functionNode = token.rootNode()[0];
			
            const auto type = functionNode.type();
            if(!type.has_value() || type != ExpressionType::FUNCTION) {
				auto ss = std::stringstream{};
				ss << "function " << token.contextNode() << " is called before being declared (or has a bad declaration)";
				throw std::runtime_error(ss.str());
            }

			const auto typeSize = type.value().size();
			assert(typeSize != 0 && "Function declaration type must have at least a return type (even if it's void)");
			const auto callNodeParameterSize = token.rootNode().size();
            if(typeSize != callNodeParameterSize) {
                auto ss = std::stringstream {};
                ss << "bad function call : the function " << functionNode << " needs " << (typeSize - 1) << " parameters but is being called with " << (callNodeParameterSize - 1) << " parameters (function type is " << type.value() << ")";
                throw std::runtime_error(ss.str());
            }

			SLOG(ska::LogLevel::Debug) << functionNode << " function has the following arguments types during its call : ";
			const auto& functionCallNode = token.rootNode();
			for (auto index = 1u; index < functionCallNode.size(); index++) {
				auto& param = functionCallNode[index];
				const auto calculatedType = param.type().value();
				const auto requiredType = (type.value().compound())[index - 1];
				//SLOG(ska::LogLevel::Debug) << "\"" << calculatedType << "\" while a type convertible to \"" << requiredType << "\" is required";

                if(requiredType.crossTypes('=', calculatedType) == ExpressionType::VOID) {
                    auto ss = std::stringstream {};
                    ss << "Type  \"" << calculatedType << "\" is encountered while a type convertible to \"" << requiredType << "\" is required";
                    throw std::runtime_error(ss.str());
                }
				index++;
            }
        } break;
    }
    
    return true;
}

bool ska::SemanticTypeChecker::matchVariable(const VarTokenEvent& token) {
    assert((token.rootNode().size() >= 1 && token.rootNode().type().has_value()) && "The current variable has no type. Maybe you forgot to add a type builder ?");
    const auto tokenNodeExpressionType = token.rootNode().type().value();
	const auto value = token.rootNode()[0].name();
    const auto variable = token.rootNode().name();
    const auto type = token.rootNode().type();
    
	SLOG(ska::LogLevel::Debug) << variable << " = " << value << ";\tsymbol = " << tokenNodeExpressionType;

    if(type.has_value() && token.type() == VarTokenEventType::AFFECTATION) {
        const auto newTokenType = type.value().crossTypes('=', tokenNodeExpressionType);
        if(newTokenType == ExpressionType::VOID) {
            const auto expressionTypeIndex = tokenNodeExpressionType;
			auto ss = std::stringstream{};
			ss << "The symbol \"" << variable << "\" has already been declared as " <<
				type.value() << " but is now wanted to be " <<
				expressionTypeIndex;
			throw std::runtime_error(ss.str());		
        }
    }

    return true;
}
