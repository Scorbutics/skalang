#include "Parser.h"
#include "SemanticTypeChecker.h"
#include "SymbolTable.h"
#include "AST.h"

//#define SKALANG_LOG_SEMANTIC_TYPE_CHECK

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
    const auto variable = token.node.token.asString();
 
    switch(token.type) {
        case FunctionTokenEventType::DECLARATION_STATEMENT:break;
        
        case FunctionTokenEventType::DECLARATION_PARAMETERS: {
            const auto parameters = token.node.size();
        
            for(auto index = 0u; index < parameters; index++) {
                auto& param = token.node[index];
                getExpressionType(param);
            }

        } break;

        default:
        case FunctionTokenEventType::CALL: {
            const auto symbol = (*m_symbols)[variable];
            if(symbol == nullptr || symbol->category != ExpressionType::FUNCTION) {
                throw std::runtime_error("function " + variable + " is called before being declared (or has a bad declaration)");
            }

            if(symbol->size() != token.node.size()) {
                auto ss = std::stringstream {};
                ss << "bad function call : the function " << variable << " needs " << symbol->size() << " parameters but is being called with " << token.node.size() << " parameters";
                throw std::runtime_error(ss.str());
            }

#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
            std::cout << variable << " function has the following arguments types during its call : " << std::endl;
#endif

            for(auto index = 0u; index < token.node.size(); index++) {
                auto& param = token.node[index];
                const auto calculatedType = getExpressionType(param);
#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
                std::cout << index << " \"" << ExpressionTypeSTR[static_cast<std::size_t>(calculatedType)] << "\" while a type convertible to \"" << ExpressionTypeSTR[static_cast<std::size_t>((*symbol)[index].category)] << "\" is required" << std::endl;
#endif
                if(crossTypes('=', (*symbol)[index].category, calculatedType) == ExpressionType::VOID) {
                    auto ss = std::stringstream {};
                    ss << "Type  \"" << ExpressionTypeSTR[static_cast<std::size_t>(calculatedType)] << "\" is encountered while a type convertible to \"" << ExpressionTypeSTR[static_cast<std::size_t>((*symbol)[index].category)] << "\" is required" << std::endl;
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
    const auto variable = token.node.token.asString();
    const auto symbol = (*m_symbols)[variable];
    
#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
    std::cout << variable << " = " << value << ";\tsymbol = " << ExpressionTypeSTR[static_cast<std::size_t>(tokenNodeExpressionType)] << std::endl;
#endif

    if(symbol != nullptr && token.type == VarTokenEventType::AFFECTATION) {
        const auto newTokenType = crossTypes('=', symbol->category, tokenNodeExpressionType);
        if(newTokenType == ExpressionType::VOID) {
            const auto expressionTypeIndex = tokenNodeExpressionType;
            throw std::runtime_error("The symbol \"" + variable + "\" has already been declared as " + 
                     std::string(ExpressionTypeSTR[static_cast<std::size_t>(symbol->category)]) + " but is now wanted to be " +
                     std::string(ExpressionTypeSTR[static_cast<std::size_t>(expressionTypeIndex)]));		
        }
    }

    return true;
}

ska::ExpressionType ska::SemanticTypeChecker::crossTypes(char op, ExpressionType type1, ExpressionType type2) {
	constexpr auto TypeMapSize = 7;

	static int typeMapOperatorPlus[TypeMapSize][TypeMapSize] = { 
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 4, 4, 0, 0 },
		{ 0, 0, 0, 5, 0, 5, 0 },
		{ 0, 0, 0, 0, 0, 0, 6 }
	};

	static int typeMapOperatorMinus[TypeMapSize][TypeMapSize] = { 
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 0, 0 },
		{ 0, 0, 0, 4, 4, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 }
	};

	static int typeMapOperatorMul[TypeMapSize][TypeMapSize] = { 
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 4, 4, 0, 0 },
		{ 0, 0, 0, 5, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 }
	};

	static int typeMapOperatorDiv[TypeMapSize][TypeMapSize] = { 
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 0, 0 },
		{ 0, 0, 0, 4, 4, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 }
	};

    static int typeMapOperatorEqual[TypeMapSize][TypeMapSize] = { 
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 1, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 0, 0, 0, 6 }
	};

	int (*selectedTypeMap)[TypeMapSize];

	switch(op) {
		case '-':
			selectedTypeMap = typeMapOperatorMinus;
			break;
		case '+':
			selectedTypeMap = typeMapOperatorPlus;
			break;
		case '/':
			selectedTypeMap = typeMapOperatorDiv;
			break;
		case '*':
			selectedTypeMap = typeMapOperatorMul;
			break;
        case '=':
            selectedTypeMap = typeMapOperatorEqual;
            break;

		default: {
			std::cout << "Unknown operator \"" << op << "\", returning by default first type (of index " << 
				static_cast<std::size_t>(type1) << ") without checking type map" << std::endl;  
		} return type1;
	}

	const auto typeIdResult = selectedTypeMap[static_cast<std::size_t>(type1)][static_cast<std::size_t>(type2)];
	if(typeIdResult == 0) {
		auto ss = std::stringstream {};
		ss << "Unable to use operator \"" << op << "\" on types " << ExpressionTypeSTR[static_cast<std::size_t>(type1)] << " and " << ExpressionTypeSTR[static_cast<std::size_t>(type2)];
		throw std::runtime_error(ss.str()); 
	}

	return static_cast<ExpressionType>(typeIdResult);

}

void ska::SemanticTypeChecker::setSymbolTable(const SymbolTable& symbolTable) {
    m_symbols = &symbolTable;
}

ska::ExpressionType ska::SemanticTypeChecker::calculateNodeExpressionType(ASTNode& node) const {
	if(node.op.has_value() && node.op.value() != Operator::LITERAL) {
		const auto& op = node.op.value();
		switch(op) {
			case Operator::FUNCTION_DECLARATION:
				return ExpressionType::FUNCTION;

			case Operator::FUNCTION_CALL: {
				const auto functionName = node.token.asString();
				const auto symbol = (*m_symbols)[functionName];
				return symbol == nullptr ? ExpressionType::VOID : symbol->category;
			}
			
			case Operator::FIELD_ACCESS:
				//TODO
				throw std::runtime_error("TODO");
			
			case Operator::BINARY: {
				assert(node.size() == 2 && !node.token.asString().empty());
				const auto type1 = getExpressionType(node[0]);
				const auto type2 = getExpressionType(node[1]);
				return crossTypes(node.token.asString()[0], type1, type2);
			}

            case Operator::PARAMETER_DECLARATION:
                //std::cout << node.asString() << " " << node.size() << std::endl;
                assert(node.size() == 1);
                if(ExpressionTypeMap.find(node[0].asString()) == ExpressionTypeMap.end()) {
                    throw std::runtime_error("bad type detected as function parameter : " + node[0].asString());
                }
                getExpressionType(node[0]);
                return ExpressionTypeMap.at(node[0].asString());

			case Operator::VARIABLE_AFFECTATION:
			case Operator::UNARY:
				assert(node.size() == 1);
				return getExpressionType(node[0]);

			default:
				return ExpressionType::VOID;
		}
	}

	const auto& token = node.token;
	switch(token.type()) {
		case TokenType::SYMBOL:
		case TokenType::SPACE:
		case TokenType::RANGE:
		case TokenType::DOT_SYMBOL:
			return ExpressionType::VOID;

		case TokenType::STRING:
			return ExpressionType::STRING;

		case TokenType::DIGIT: {
			const auto isDecimal = token.asString().find_first_of('.') != std::string::npos;
            return isDecimal ? ExpressionType::FLOAT : ExpressionType::INT;
        }

		case TokenType::IDENTIFIER: {
			const auto symbol = (*m_symbols)[token.asString()];
			return symbol == nullptr ? ExpressionType::VOID : symbol->category;
		}
		default:
			break;
	}

	return ExpressionType::VOID;
}

ska::ExpressionType ska::SemanticTypeChecker::getExpressionType(ASTNode& node) const {
	if(node.type.has_value()) {
		return node.type.value();
	}

	const auto expressionType = calculateNodeExpressionType(node);
	node.type = expressionType;
	return expressionType;
}

