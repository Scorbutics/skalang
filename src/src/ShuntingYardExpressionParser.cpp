#include <iostream>
#include "LoggerConfigLang.h"
#include "ShuntingYardExpressionParser.h"
#include "Parser.h"
#include "AST.h"
#include "ReservedKeywordsPool.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ShuntingYardExpressionParser)

template <class Container>
void Print(const Container& c, const std::string& name = " ") {
	std::cout << name << " : ";
	for(const auto& it : c) {
		std::cout << it.asString() << " ";
	}
	std::cout << std::endl;
}

template <class Container>
void PrintPtr(const Container& c, const std::string& name = " ") {
	std::cout << name << " : ";
	for(const auto& it : c) {
		if(it != nullptr) {
			std::cout << it->asString() << " ";
		}
	}
	std::cout << std::endl;
}

std::unordered_map<char, int> ska::ShuntingYardExpressionParser::BuildPriorityMap() {
	auto result = std::unordered_map<char, int>{};

    result.emplace('=', 50);
    result.emplace('>', 50);
    result.emplace('<', 50);
	result.emplace('+', 100);
	result.emplace('-', 100);
	result.emplace('*', 200);
	result.emplace('/', 200);

	return result;
}

std::unordered_map<char, int> ska::ShuntingYardExpressionParser::PRIORITY_MAP = BuildPriorityMap();

ska::ShuntingYardExpressionParser::ShuntingYardExpressionParser(const ReservedKeywordsPool& reservedKeywordsPool, Parser& parser, TokenReader& input) :
	m_reservedKeywordsPool(reservedKeywordsPool),
	m_parser(parser),
	m_input(input) {
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::parse() {
	auto operators = stack<Token>{};
	auto operands = stack<ASTNodePtr> {};
	return expression(operators, operands);
}

bool ska::ShuntingYardExpressionParser::parseTokenExpression(stack<Token>& operators, stack<ASTNodePtr>& operands, const Token& token) {

	switch (token.type()) {
		case TokenType::RESERVED: {
			SLOG(ska::LogLevel::Debug) << "\tPushing reserved";
			auto reservedNode = matchReserved();
			if (reservedNode != nullptr) {
				operands.push(std::move(reservedNode));
				break;
			}
		}
		case TokenType::IDENTIFIER:
		case TokenType::STRING:
		case TokenType::DIGIT:
			SLOG(ska::LogLevel::Debug) << "\tPushing operand " << token;
			operands.push(ASTNode::MakeLogicalNode(m_input.match(token.type())));
			return true;

		case TokenType::RANGE: {
			const auto& value = std::get<std::string>(token.content());
			switch (value[0]) {
			case '(':
				
				if (!operands.empty() && (operands.top()->op() == Operator::FIELD_ACCESS || 
					(operands.top()->op() == Operator::LITERAL || operands.top()->op() == Operator::UNARY) && operands.top()->tokenType() == TokenType::IDENTIFIER)) {
					SLOG(ska::LogLevel::Debug) << "\tFunction call !";
					//We already pushed the identifier as an operand, but in fact it's a function call.
					//We have to pop it, then matching the function call as the new operand.
					auto functionToCallNode = std::move(operands.top());
					operands.pop();
					operands.push(matchFunctionCall(std::move(functionToCallNode)));
					
				} else {
					SLOG(ska::LogLevel::Debug) << "\tRange begin";
					operators.emplace(m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>()));
				}
				break;
			case ')': {
				operators.emplace(m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>()));
				SLOG(ska::LogLevel::Debug) << "\tRange end";
				auto rangeOperandResult = popUntil(operators, operands, [](const Token& t) {
					const auto& strValue = t.name();
					if (t.type() == TokenType::RANGE) {
						return (strValue.empty() || strValue[0] == '(') ? -1 : 1;
					}
					SLOG_STATIC(ska::LogLevel::Debug, ska::ShuntingYardExpressionParser) << "\t\tPoping " << strValue;
					return 0;
				});
				if (rangeOperandResult != nullptr) {
					operands.push(std::move(rangeOperandResult));
				}
				if (!operators.empty()) {
					operators.pop();
				}
			}
			break;

			default:
				error("Unexpected token (range type) : " + token.name());
			}

		}
		break;
		
		case TokenType::DOT_SYMBOL: {
			//Field access only (digits are DIGIT token type, even real ones)
			auto lastNode = std::move(operands.top());
			operands.pop();
			operands.push(matchObjectFieldAccess(std::move(lastNode)));
			return true;
		}
		case TokenType::SYMBOL: {
			const auto& value = std::get<std::string>(token.content());
			if(value == "=") {
				auto lastToken = ska::Token{ operands.top()->name(), operands.top()->tokenType() };
				operands.pop();
				operands.push(matchAffectation(lastToken));
			} else {
				SLOG(ska::LogLevel::Debug) << "\tPushing operator symbol " << value;
				const auto shouldPopOperatorsStack = checkLessPriorityToken(operators, operands, value[0]);
				auto operatorTop = operators.empty() ? Token{} : operators.top();
				if (shouldPopOperatorsStack) {
					SLOG(ska::LogLevel::Debug) << "\tLess precedence, poping " << operatorTop << " before adding " << value;
					auto poped = 0u;
					auto popedToken = popUntil(operators, operands, [&](const Token& t) {
						if (poped >= 1) {
							return -1;
						}
						poped++;
						return 0;
					});
					assert(popedToken != nullptr);
					operands.push(std::move(popedToken));
				}
				operators.emplace(m_input.match(TokenType::SYMBOL));
			}
		}
		break;

	default:
		error("Expected a symbol, a litteral, an identifier or a reserved keyword, but got the token : " + token.name());
	}
	return false;
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::matchFunctionCall(ASTNodePtr identifierFunctionName) {
	//First match left parenthesis
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

	//auto functionName = identifierFunctionName->name();

	auto functionCallNodeContent = std::vector<ASTNodePtr>{};
	functionCallNodeContent.push_back(std::move(identifierFunctionName));
	const auto endParametersToken = m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>();
	while (!m_input.expect(endParametersToken)) {
		
		const auto commaToken = m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>();
		if(m_input.expect(commaToken)) {
			m_input.match(commaToken);
		}

		auto expressionOpt = parse();
		if (expressionOpt != nullptr) {
			SLOG(ska::LogLevel::Debug) << "Expression not null";
			functionCallNodeContent.push_back(std::move(expressionOpt));
		} else {
			SLOG(ska::LogLevel::Debug) << "Expression null";
			break;
		}
	}
	m_input.match(endParametersToken);

	auto functionCallNode = ASTNode::MakeNode<Operator::FUNCTION_CALL>(std::move(functionCallNodeContent));
	auto event = FunctionTokenEvent { *functionCallNode, (*functionCallNode)[0], FunctionTokenEventType::CALL };
	m_parser.Observable<FunctionTokenEvent>::notifyObservers(event);
	return functionCallNode;
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::matchObjectFieldAccess(ASTNodePtr objectAccessed) {
	
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::METHOD_CALL_OPERATOR>());
    auto fieldAccessedIdentifier = m_input.match(TokenType::IDENTIFIER);

    auto fieldAccessNode = ASTNode::MakeNode<Operator::FIELD_ACCESS>(std::move(objectAccessed), ASTNode::MakeLogicalNode(fieldAccessedIdentifier));
    
    /*
	auto event = VarTokenEvent { *fieldAccessNode, VarTokenEventType::USE };
	m_parser.Observable<VarTokenEvent>::notifyObservers(event);
	*/
	return fieldAccessNode;
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::matchAffectation(Token identifierFieldAffected) {
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::AFFECTATION>());
	auto expressionNode = parse();
	if(expressionNode == nullptr) {
		error("Affectation incomplete : no expression");
	}
	
	auto affectationNode = ASTNode::MakeNode<Operator::VARIABLE_AFFECTATION>(ASTNode::MakeLogicalNode(std::move(identifierFieldAffected)), std::move(expressionNode));
	auto event = VarTokenEvent { *affectationNode, VarTokenEventType::AFFECTATION };
	m_parser.Observable<VarTokenEvent>::notifyObservers(event);
	return affectationNode;
}

bool ska::ShuntingYardExpressionParser::isAtEndOfExpression() const {
    return m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>()) || 
            m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>()) ||
            m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>()); 
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::expression(stack<Token>& operators, stack<ASTNodePtr>& operands) {
	auto rangeCounter = 0;

    while (!isAtEndOfExpression() && rangeCounter >= 0) {
		//PrintPtr(operands, "Operands");
		//Print(operators, "Operators");
        
		rangeCounter += m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>()) ? 1 : 0;
		rangeCounter -= m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>()) ? 1 : 0;
		
        auto token = m_input.actual();
		if (rangeCounter >= 0) {
			parseTokenExpression(operators, operands, token);
			if (!operands.empty() && operands.top()->op() == Operator::FUNCTION_CALL) {
				rangeCounter = 0;
			}
		}
	}
	SLOG(ska::LogLevel::Debug) << "\tPoping everything";

	auto result = popUntil(operators, operands, [](const auto& t) {
		return 0;
	});

	auto event = ExpressionTokenEvent { *result };
	m_parser.Observable<ExpressionTokenEvent>::notifyObservers(event);

	return result;
}

bool ska::ShuntingYardExpressionParser::checkLessPriorityToken(stack<Token>& operators, stack<ASTNodePtr>& operands, const char tokenChar) const {
	const auto& topOperatorContent = (operators.empty() || operators.top().name().empty()) ? '\0' : operators.top().name()[0];
    if(PRIORITY_MAP.find(tokenChar) == PRIORITY_MAP.end()) {
        auto ss = std::stringstream {};
        ss << "bad operator : " << tokenChar;
        error(ss.str());
    }

    return PRIORITY_MAP.find(topOperatorContent) != PRIORITY_MAP.end() &&
			PRIORITY_MAP.at(tokenChar) < PRIORITY_MAP.at(topOperatorContent);
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::matchReserved() {
	const auto& result = m_input.actual();

	switch(std::get<std::size_t>(result.content())) {
		case static_cast<std::size_t>(TokenGrammar::FUNCTION):
			return matchFunctionDeclaration();

		default:
			return nullptr;
	}
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::matchFunctionDeclarationParameter() {
	const auto isRightParenthesis = m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	if(isRightParenthesis) {
		return nullptr;
	}
	const auto& id = m_input.match(TokenType::IDENTIFIER);
	SLOG(ska::LogLevel::Debug) << id.name();
	
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
    const auto typeToken = 
		m_input.expect(TokenType::RESERVED) ?
		m_input.match(TokenType::RESERVED) : m_input.match(TokenType::IDENTIFIER);
	const auto typeStr = typeToken.name();

	SLOG(ska::LogLevel::Debug) << "type is : " << typeStr;
	
	//TODO handle arrays
	auto node = ASTNode::MakeNode<Operator::PARAMETER_DECLARATION>(id, ASTNode::MakeLogicalNode(typeToken));
    return node;
}

std::vector<ska::ASTNodePtr> ska::ShuntingYardExpressionParser::fillFunctionDeclarationParameters() {
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

	auto parameters = std::vector<ASTNodePtr>{};
	auto isRightParenthesis = m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	auto isComma = true;
	while (!isRightParenthesis && isComma) {
		if (!m_input.expect(TokenType::SYMBOL)) {
			SLOG(ska::LogLevel::Debug) << "parameter detected, reading identifier : ";
			auto parameterNode = matchFunctionDeclarationParameter();
			parameters.push_back(std::move(parameterNode));
			isComma = m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			if (isComma) {
				m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
			}
		}
		isRightParenthesis = m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	}
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
	
	return parameters;
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::matchFunctionDeclarationReturnType() {
	if (m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>())) {
		m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
		const auto type = m_input.match(TokenType::RESERVED);
		SLOG(ska::LogLevel::Debug) << "function type detected : " << type;
		//if(type.asString() != "var") {
		return ASTNode::MakeLogicalNode(type);
		//}
	} else {
		SLOG(ska::LogLevel::Debug) << "void function detected";
		return ASTNode::MakeLogicalNode(ska::Token{ "", ska::TokenType::IDENTIFIER });
	}
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::matchFunctionDeclarationBody() {
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());

	auto statements = std::vector<ASTNodePtr>{};
	while (!m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>())) {
		auto optionalStatement = m_parser.statement();
		if (!optionalStatement->logicalEmpty()) {
			statements.push_back(std::move(optionalStatement));
		} else {
			break;
		}
	}
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());

	auto blockNode = ASTNode::MakeNode<Operator::BLOCK>(std::move(statements));
	return blockNode;
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::matchFunctionDeclaration() {
	SLOG(ska::LogLevel::Debug) << "function declaration";
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::FUNCTION>());

    //With this grammar, no other way than reading previously to retrieve the function name.
    const auto functionName = m_input.readPrevious(3); 

	auto contentNode = fillFunctionDeclarationParameters();
	
    auto parametersNode = ASTNode::MakeNode<Operator::PARAMETER_PACK_DECLARATION>(std::move(contentNode));
    auto returnTypeNode = matchFunctionDeclarationReturnType();

	auto startEvent = FunctionTokenEvent{ *parametersNode, *returnTypeNode, FunctionTokenEventType::DECLARATION_PARAMETERS, functionName.name() };
	m_parser.Observable<FunctionTokenEvent>::notifyObservers(startEvent);

    SLOG(ska::LogLevel::Debug) << "reading function body";
	auto functionBodyNode = matchFunctionDeclarationBody();
	SLOG(ska::LogLevel::Debug) << "function read.";
	
	auto functionDeclarationNode = ASTNode::MakeNode<Operator::FUNCTION_DECLARATION>(functionName, std::move(parametersNode), std::move(returnTypeNode), std::move(functionBodyNode));
	

	auto endEvent = FunctionTokenEvent {*functionDeclarationNode, *returnTypeNode, FunctionTokenEventType::DECLARATION_STATEMENT, functionName.name() };
	m_parser.Observable<FunctionTokenEvent>::notifyObservers(endEvent);

	return functionDeclarationNode;
}

ska::ASTNodePtr ska::ShuntingYardExpressionParser::popUntil(stack<Token>& operators, stack<ASTNodePtr>& operands, PopPredicate predicate) {
	auto currentNode = ASTNodePtr {};
	auto nextNode = ASTNodePtr {};

	if (operators.empty() && !operands.empty()) {
		auto result = std::move(operands.top());
		operands.pop();
		return result;
	}

	while(true) {
		if (operators.empty() || operands.empty()) {
			break;
		}

		const auto op = operators.top();
		const auto analyzeToken = predicate(op);
		//Flow control loop predicate by token
		if(analyzeToken < 0) {
			break;
		} else if (analyzeToken > 0) {
			operators.pop();
			continue;
		}
		operators.pop();

		auto rightOperand = std::move(operands.top());
		operands.pop();

        currentNode = ASTNodePtr{};
		if(nextNode != nullptr) {
			currentNode = ASTNode::MakeLogicalNode(op, std::move(nextNode), std::move(rightOperand));
		} else if(!operands.empty()) {
			currentNode = ASTNode::MakeLogicalNode(op, std::move(operands.top()), std::move(rightOperand)); operands.pop();
		} else {
			//TODO handle unary operator ?
            //assert(false && "Unsupported for now");
            //SLOG(ska::LogLevel::Debug) << "\t\tOperator " << op.asString();
            currentNode = ASTNode::MakeLogicalNode(op, /*ASTNode::MakeLogicalNode(Token { "0", TokenType::DIGIT }),*/ std::move(rightOperand));
		}

		nextNode = std::move(currentNode);
		SLOG(ska::LogLevel::Debug) << "\t\tPoped " << op;//<< " with " <<  nextNode->left->token.asString() << " and " << nextNode->right->token.asString();
	}

	return (currentNode == nullptr  && nextNode == nullptr) ? nullptr : std::move(nextNode);
}

void ska::ShuntingYardExpressionParser::error(const std::string& message) {
	throw std::runtime_error("syntax error : " + message);
}



