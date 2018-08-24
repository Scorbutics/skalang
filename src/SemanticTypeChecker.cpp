#include "SemanticTypeChecker.h"
#include "SymbolTable.h"
#include "AST.h"

#define SKALANG_LOG_SEMANTIC_TYPE_CHECK

ska::SemanticTypeChecker::SemanticTypeChecker(Observable<ExpressionTokenEvent>& expressionDeclarer, Observable<VarTokenEvent>& varDeclarer) :
	SubObserver<ExpressionTokenEvent>(std::bind(&SemanticTypeChecker::matchExpression, this, std::placeholders::_1), expressionDeclarer),
    SubObserver<VarTokenEvent>(std::bind(&SemanticTypeChecker::matchVariable, this, std::placeholders::_1), varDeclarer){

}

bool ska::SemanticTypeChecker::matchExpression(ExpressionTokenEvent& token) {
	getExpressionType(token.node);
    return true;
}

bool ska::SemanticTypeChecker::matchVariable(VarTokenEvent& token) {
    const auto tokenNodeExpressionType = getExpressionType(token.node[0]);
	const auto value = token.node[0].asString();
    const auto variable = token.node.token.asString();
    const auto symbol = (*m_symbols)[variable];
    
#ifdef SKALANG_LOG_SEMANTIC_TYPE_CHECK
    std::cout << variable << " = " << value << ";\tsymbol = " << ExpressionTypeSTR[static_cast<std::size_t>(tokenNodeExpressionType)] << 
        ";\told symbol = " << (symbol == nullptr ? "nullptr" : ExpressionTypeSTR[static_cast<std::size_t>(symbol->category)]) << std::endl;
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
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 3, 4, 5, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 }
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

