#include "Parser.h"
#include "SemanticTypeChecker.h"
#include "SymbolTable.h"
#include "AST.h"

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
                ss << "bad function call : the function " << variable << " needs " << (symbol->size() - 1) << " parameters but is being called with " << (token.node.size() - 1) << " parameters";
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
    const auto tokenNodeExpressionType = getExpressionType(token.node[0]);
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

ska::Type ska::SemanticTypeChecker::calculateNodeExpressionType(ASTNode& node) const {
	if(node.op.has_value() && node.op.value() != Operator::LITERAL) {
		const auto& op = node.op.value();
		switch(op) {
			case Operator::FUNCTION_DECLARATION: {
				auto type = Type{ ExpressionType::FUNCTION };
				for (auto index = 0u; index < node.size() - 1; index++) {
					type.add(getExpressionType(node[index]));
				}
                if(node.size() > 1) {
                    auto returnType = getExpressionType(node[node.size() - 1]);
                    if(returnType == ExpressionType::OBJECT) {
                        returnType.name(node.asString());
                        //std::cout << "!!!! return type of function " << node.asString() << " : " << returnType.asString() << std::endl; 
                        type.add(returnType);
                    }
                }
#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
                std::cout << "function declaration \""<< node.asString() <<"\" with type "<< type.asString() << std::endl;
#endif
                return type;
			}

			case Operator::FUNCTION_CALL: {
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
			
			case Operator::FIELD_ACCESS: {
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

			case Operator::BINARY: {
				assert(node.size() == 2 && !node.asString().empty());
				const auto type1 = getExpressionType(node[0]);
				const auto type2 = getExpressionType(node[1]);
				return type1.crossTypes(node.asString()[0], type2);
			}

			case Operator::PARAMETER_DECLARATION: {
				//std::cout << node.asString() << " " << node.size() << std::endl;
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

			case Operator::VARIABLE_DECLARATION:
			case Operator::UNARY: {
				assert(node.size() == 1);
				const auto childType = getExpressionType(node[0]);
                return childType;
			}

            case Operator::VARIABLE_AFFECTATION: {                                    
                const auto varTypeSymbol = (*m_symbols)[node.asString()];
                assert(varTypeSymbol != nullptr);
                return varTypeSymbol->getType();
            }
			

            default:
				return ExpressionType::VOID;
		}
	}

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

ska::Type ska::SemanticTypeChecker::getExpressionType(ASTNode& node) const {
	if(node.type.has_value()) {
		return node.type.value();
	}

	const auto expressionType = calculateNodeExpressionType(node);
	node.type = expressionType;
	return expressionType;
}

