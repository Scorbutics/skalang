#include "Parser.h"
#include "SemanticTypeChecker.h"
#include "SymbolTable.h"
#include "AST.h"

#include "TypeBuilder.h"

#define SKALANG_LOG_SEMANTIC_TYPE_CHECK

ska::SemanticTypeChecker::SemanticTypeChecker(Parser& parser) :
	SubObserver<ExpressionTokenEvent>(std::bind(&SemanticTypeChecker::matchExpression, this, std::placeholders::_1), parser),
    SubObserver<VarTokenEvent>(std::bind(&SemanticTypeChecker::matchVariable, this, std::placeholders::_1), parser),
    SubObserver<FunctionTokenEvent>(std::bind(&SemanticTypeChecker::matchFunction, this, std::placeholders::_1), parser) {
}

bool ska::SemanticTypeChecker::matchExpression(ExpressionTokenEvent& token) {
	getExpressionType(token.node);
    return true;
}

bool ska::SemanticTypeChecker::matchFunction(FunctionTokenEvent& token) {
    const auto variable = token.node[0].asString();
 
    switch(token.type) {
        case FunctionTokenEventType::DECLARATION_STATEMENT:break;
        
        case FunctionTokenEventType::DECLARATION_PARAMETERS: {			
			getExpressionType(token.node);
			const auto returnType = token.node[token.node.size() - 1].type;
#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
            if (returnType.has_value() && returnType.value() == ExpressionType::OBJECT) {
				std::cout << "user defined object type detected" << std::endl;
			}
#endif
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

#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
            std::cout << variable << " function has the following arguments types during its call : " << std::endl;
#endif

            for(auto index = 1u; index < token.node.size(); index++) {
                auto& param = token.node[index];
                const auto calculatedType = getExpressionType(param);
#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
                std::cout << index << " \"" << calculatedType.asString() << "\" while a type convertible to \"" << (*symbol)[index].asString() << "\" is required" << std::endl;
#endif
                if((*symbol)[index - 1].crossTypes('=', calculatedType) == ExpressionType::VOID) {
                    auto ss = std::stringstream {};
                    ss << "Type  \"" << calculatedType.asString() << "\" is encountered while a type convertible to \"" << (*symbol)[index].asString() << "\" is required" << std::endl;
                    throw std::runtime_error(ss.str());
                }
            }
        } break;
    }
    
    return true;
}

bool ska::SemanticTypeChecker::matchVariable(VarTokenEvent& token) {
    assert(token.node.size() >= 1 && token.node[0].has_value());
    const auto tokenNodeExpressionType = token.node[0].value();
	const auto value = token.node[0].asString();
    const auto variable = token.node.asString();
    const auto symbol = (*m_symbols)[variable];
    
#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
    std::cout << variable << " = " << value << ";\tsymbol = " << tokenNodeExpressionType.asString() << std::endl;
#endif

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



// TODO
// MOVE
// THOSE
// BOTTOM
// LINES

ska::Type ska::SemanticTypeChecker::buildTypeFunctionDeclaration(ASTNode& node) const {
    auto functionType = Type{ ExpressionType::FUNCTION };
    for (auto index = 0u; index < node.size(); index++) {
        auto varType = getExpressionType(node[index]);
        if(varType == ExpressionType::OBJECT) {
            varType.name(node.asString());
        }
        functionType.add(std::move(varType));
    }
#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
    std::cout << "function declaration \""<< node.asString() <<"\" with type "<< functionType.asString() << std::endl;
#endif
    return functionType;
}

ska::Type ska::SemanticTypeChecker::buildTypeFunctionCall(ASTNode& node) const {
    const auto& functionIdentifier = node[0];
    const auto type = getExpressionType(node[0]);
    const auto functionName = functionIdentifier.asString();
    auto* symbol = (*m_symbols)[functionName];
    auto* n = &node[0];
    auto* currentSymbolTable = symbol->getType().symbolTable();
    while (n != nullptr && n->size() > 0 && !currentSymbolTable->children().empty()) {
        n = &(*n)[0];
        currentSymbolTable = currentSymbolTable->children()[0].get();
        const auto& fieldName = n->asString();
        auto* fieldSymbol = (*currentSymbolTable)[fieldName];
#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
        std::cout << "field symbol : " << (fieldSymbol != nullptr ? fieldSymbol->getType().asString() : "null") << std::endl;
#endif
        symbol = fieldSymbol;
    }
#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
    if(symbol != nullptr) {
        std::cout << "function call with type : " << symbol->getType().asString() << std::endl;
    }
#endif
    if(symbol == nullptr || symbol->empty() || (*symbol)[0] == ExpressionType::VOID) {
        return ExpressionType::VOID;
    }

    //std::cout << "returning type : " << symbol->category.compound().back().asString() << " of " << symbol->category.asString() << std::endl;
    return symbol->getType().compound().back();

}

ska::Type ska::SemanticTypeChecker::buildTypeFieldAccess(ASTNode& node) const {
    const auto symbolObject = (*m_symbols)[node.asString()];
    if (symbolObject == nullptr) {
        throw std::runtime_error("trying to dereference an unknown symbol : " + node.asString());
    }
    
    const auto fieldAccessed = node[0].asString();
    const auto symbolField = (*symbolObject)[fieldAccessed];
    if (symbolField == nullptr) {
        throw std::runtime_error("trying to access to an undeclared field : " + fieldAccessed + " of " + node.asString() + (symbolObject->empty() ? (" of type " + symbolObject->getType().asString()) :  " of type " + (*symbolObject)[0].asString()));
    }
    return (*symbolObject)[0];

}

ska::Type ska::SemanticTypeChecker::buildTypeBinary(ASTNode& node) const {
    assert(node.size() == 2 && !node.asString().empty());
    const auto type1 = getExpressionType(node[0]);
    const auto type2 = getExpressionType(node[1]);
    return type1.crossTypes(node.asString()[0], type2);
}

ska::Type ska::SemanticTypeChecker::buildTypeParameterDeclaration(ASTNode& node) const {
    assert(node.size() == 1);
    const auto typeStr = node[0].asString();

    if (ExpressionTypeMap.find(typeStr) == ExpressionTypeMap.end()) {
        const auto symbolType = (*m_symbols)[typeStr];
        if (symbolType == nullptr) {
            throw std::runtime_error("unknown type detected as function parameter : " + node[0].asString());
        }
        return (node[0].type = symbolType->getType()).value();
   } else {
        getExpressionType(node[0]);
        return ExpressionTypeMap.at(node[0].asString());
   }
}

ska::Type ska::SemanticTypeChecker::buildTypeVariableDeclaration(ASTNode& node) const {
    assert(node.size() == 1);
    const auto typeStr = node[0].asString();
    
    if (ExpressionTypeMap.find(typeStr) == ExpressionTypeMap.end()) {
        const auto symbolType = (*m_symbols)[typeStr];
        if (symbolType == nullptr) {
            throw std::runtime_error("unknown type detected as function parameter : " + node[0].asString());
        }
        return (node[0].type = symbolType->getType()).value();
    } else {
        getExpressionType(node[0]);
        return ExpressionTypeMap.at(node[0].asString());
    }
}

ska::Type ska::SemanticTypeChecker::buildTypeUnary(ASTNode& node) const {
    assert(node.size() == 1);
    const auto childType = getExpressionType(node[0]);
    return childType;
}

ska::Type ska::SemanticTypeChecker::buildTypeVariableAffectation(ASTNode& node) const {
    const auto varTypeSymbol = (*m_symbols)[node.asString()];
    assert(varTypeSymbol != nullptr);
    return varTypeSymbol->getType();
}

ska::Type ska::SemanticTypeChecker::buildTypeFromTokenType(ASTNode& node) const {
    switch(node.tokenType()) {
		case TokenType::SYMBOL:
		case TokenType::SPACE:
		case TokenType::RANGE:
		case TokenType::DOT_SYMBOL:
			return ExpressionType::VOID;

		case TokenType::STRING:
			return ExpressionType::STRING;

		case TokenType::DIGIT: {
			const auto isDecimal = node.asString().find_first_of('.') != std::string::npos;
            return isDecimal ? ExpressionType::FLOAT : ExpressionType::INT;
        }

		case TokenType::IDENTIFIER: {
			const auto symbol = (*m_symbols)[node.asString()];
			return symbol == nullptr ? ExpressionType::VOID : symbol->getType();
		}
        case TokenType::RESERVED: {
              if(node.asString() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::INT)])) {
                  return ExpressionType::INT;
              } else if (node.asString() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::FLOAT)])) {
                return ExpressionType::FLOAT;
              } else if (node.asString() == std::string(ExpressionTypeSTR[static_cast<std::size_t>(ExpressionType::STRING)])) {
                    return ExpressionType::STRING;
              } else if(node.asString() == "var") {
                  return ExpressionType::OBJECT;
              }
        }

		default:
			break;
	}

    std::cout << "default type returned for node \"" << node.asString() << "\" of type " << TokenTypeSTR[static_cast<std::size_t>(node.tokenType())] << std::endl;

	return ExpressionType::VOID;
}



