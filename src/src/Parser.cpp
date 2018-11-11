#include <iostream>
#include "LoggerConfigLang.h"
#include "AST.h"
#include "Operator.h"
#include "Parser.h"
#include "ReservedKeywordsPool.h"

//#define SKALANG_LOG_PARSER

ska::Parser::Parser(const ReservedKeywordsPool& reservedKeywordsPool, TokenReader& input) :
	m_input(input),
	m_reservedKeywordsPool(reservedKeywordsPool),
	m_shuntingYardParser(reservedKeywordsPool, *this, m_input) {
}

ska::Parser::ASTNodePtr ska::Parser::parse() {
    if(m_input.empty()) {
		return nullptr;
	}

	auto blockNodeStatements = std::vector<ASTNodePtr>{};
    while (!m_input.empty()) {
		auto optionalStatement = optstatement();
		if (optionalStatement != nullptr && !optionalStatement->logicalEmpty()) {
			blockNodeStatements.push_back(std::move(optionalStatement));
		} else {
			break;
		}
	}
	return ASTNode::MakeNode<Operator::BLOCK>(std::move(blockNodeStatements));
}

ska::Parser::ASTNodePtr ska::Parser::statement() {
	if (m_input.empty()) {
		return nullptr;
	}

	const auto token = m_input.actual();
	switch (token.type()) {
	case TokenType::RESERVED:
		return matchReservedKeyword(std::get<std::size_t>(token.content()));

	case TokenType::RANGE:
		return matchBlock(std::get<std::string>(token.content()));

	default:
        	return matchExpressionStatement();
	}
}

ska::Parser::ASTNodePtr ska::Parser::matchExpressionStatement() {
#ifdef SKALANG_LOG_PARSER
	std::cout << "Expression-statement found" << std::endl;
#endif
    auto expressionResult = expr();
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    if(expressionResult == nullptr) {
#ifdef SKALANG_LOG_PARSER
	std::cout << "NOP statement" << std::endl;
#endif
	return nullptr;
    }
    return expressionResult;
}

ska::Parser::ASTNodePtr ska::Parser::matchBlock(const std::string& content) {
	if (content == m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>().asString()) {
#ifdef SKALANG_LOG_PARSER	
		std::cout << "block start detected" << std::endl;
#endif
		m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());
		
		auto blockNode = ASTNode::MakeNode<Operator::BLOCK>();
		auto startEvent = BlockTokenEvent { *blockNode, BlockTokenEventType::START };
		Observable<BlockTokenEvent>::notifyObservers(startEvent);

        auto blockNodeStatements = std::vector<ASTNodePtr>{};
		while (!m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>())) {
			auto optionalStatement = optstatement();
			if (!optionalStatement->logicalEmpty()) {
				blockNodeStatements.push_back(std::move(optionalStatement));
			} else {
				break;
			}
		}

		blockNode = ASTNode::MakeNode<Operator::BLOCK>(std::move(blockNodeStatements));

		m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());
#ifdef SKALANG_LOG_PARSER
		std::cout << "block end" << std::endl;
#endif
		auto endEvent = BlockTokenEvent { *blockNode, BlockTokenEventType::END };
		Observable<BlockTokenEvent>::notifyObservers(endEvent);
		return blockNode;
	} else if (content == m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>().asString()) {
		error("Block end token encountered when not expected");
	} else {
		return matchExpressionStatement();
	}

	optexpr(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
	return nullptr;
}

ska::Parser::ASTNodePtr ska::Parser::matchForKeyword() {
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::FOR>());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

#ifdef SKALANG_LOG_PARSER
    std::cout << "1st for loop expression (= statement)" << std::endl;
#endif

    auto forNodeFirstExpression = optstatement();

#ifdef SKALANG_LOG_PARSER
    std::cout << "2nd for loop expression" << std::endl;
#endif

    auto forNodeMidExpression = optexpr(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());

#ifdef SKALANG_LOG_PARSER
    std::cout << "3rd for loop expression" << std::endl;
#endif

    auto forNodeLastExpression = optexpr(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

    auto forNodeStatement = statement();

#ifdef SKALANG_LOG_PARSER
    std::cout << "end for loop statement" << std::endl;
#endif

    auto forNode = ASTNode::MakeNode<Operator::FOR_LOOP>(std::move(forNodeFirstExpression), std::move(forNodeMidExpression), std::move(forNodeLastExpression), std::move(forNodeStatement));
    
    auto event = ForTokenEvent {*forNode};
    Observable<ForTokenEvent>::notifyObservers(event);
    
    return forNode;
}

ska::Parser::ASTNodePtr ska::Parser::matchIfOrIfElseKeyword() {
    auto ifNode = ASTNodePtr{};

    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::IF>());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());
    
    {
        auto conditionExpression = expr();
        m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

        auto conditionStatement = statement();

        const auto elseToken = m_reservedKeywordsPool.pattern<TokenGrammar::ELSE>();
        if (m_input.expect(elseToken)) {
            m_input.match(elseToken);
            auto elseBlockStatement = statement();
            ifNode = ASTNode::MakeNode<Operator::IF_ELSE>(std::move(conditionExpression), std::move(conditionStatement), std::move(elseBlockStatement));
        } else {
            ifNode = ASTNode::MakeNode<Operator::IF>(std::move(conditionExpression), std::move(conditionStatement));
        }
    }

    auto event = IfElseTokenEvent {*ifNode};
    Observable<IfElseTokenEvent>::notifyObservers(event);
    return ifNode;
}

ska::Parser::ASTNodePtr ska::Parser::matchVarKeyword() {
#ifdef SKALANG_LOG_PARSER
	std::cout << "variable declaration" << std::endl;
#endif
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::VARIABLE>());
	auto varNodeIdentifier = m_input.match(TokenType::IDENTIFIER);
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::AFFECTATION>());
#ifdef SKALANG_LOG_PARSER
    std::cout << "equal sign matched, reading expression" << std::endl;
#endif
    auto varNodeExpression = expr();

    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
#ifdef SKALANG_LOG_PARSER
    std::cout << "expression end with symbol ;" << std::endl;
#endif
    auto varNode = ASTNode::MakeNode<Operator::VARIABLE_DECLARATION>(std::move(varNodeIdentifier), std::move(varNodeExpression));
    
    auto event = VarTokenEvent {*varNode};
    Observable<VarTokenEvent>::notifyObservers(event);

    return varNode;
}

ska::Parser::ASTNodePtr ska::Parser::matchReservedKeyword(const std::size_t keywordIndex) {
	switch (keywordIndex) {
	case static_cast<std::size_t>(TokenGrammar::FOR):
		return matchForKeyword();

	case static_cast<std::size_t>(TokenGrammar::IF):
		return matchIfOrIfElseKeyword();

	case static_cast<std::size_t>(TokenGrammar::VARIABLE):
		return matchVarKeyword();

	case static_cast<std::size_t>(TokenGrammar::FUNCTION):
		return expr();

    case static_cast<std::size_t>(TokenGrammar::RETURN):
        return matchReturnKeyword();

	default: {
			std::stringstream ss;
			ss << (keywordIndex < static_cast<std::size_t>(TokenGrammar::UNUSED_Last_Length) ? TokenGrammarSTR[keywordIndex] : "UNKNOWN TYPE" );
			error("Unhandled keyword type : " + ss.str());
			return nullptr;
		}
	}
}

ska::Parser::ASTNodePtr ska::Parser::matchReturnKeyword() {
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::RETURN>());

    //TODO handle native (= built-in) types

    //std::cout << "return detected" << std::endl;
    
    auto returnFieldNodes = std::vector<ASTNodePtr>{};

    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());
    while(!m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>())) {
        auto field = m_input.match(TokenType::IDENTIFIER);
        m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
        auto fieldValue = expr();

        const std::string name = "???";
#ifdef SKALANG_LOG_PARSER
        std::cout << "Constructor " << name << " with field \"" << field.asString() << "\" and field value \"" << fieldValue->asString() << "\"" <<  std::endl;
#endif
        
        auto fieldNode = ASTNode::MakeNode<Operator::VARIABLE_DECLARATION>(std::move(field), std::move(fieldValue));
		
        auto event = VarTokenEvent{ *fieldNode };
		Observable<VarTokenEvent>::notifyObservers(event);
		
		returnFieldNodes.push_back(std::move(fieldNode));

		if (m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>())) {
			m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
		}
    }
    
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    
    auto returnNode = ASTNode::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(returnFieldNodes)); 
    auto event = ReturnTokenEvent { *returnNode, ReturnTokenEventType::OBJECT };
    Observable<ReturnTokenEvent>::notifyObservers(event);
    return returnNode;
}

ska::Parser::ASTNodePtr ska::Parser::expr() {
	return m_shuntingYardParser.parse();
}

void ska::Parser::error(const std::string& message) {
	throw std::runtime_error("syntax error : " + message);
}

ska::Parser::ASTNodePtr ska::Parser::optexpr(const Token& mustNotBe) {
	auto node = ASTNodePtr {};
	if (!m_input.expect(mustNotBe)) {
		node = expr();
	}
	return node != nullptr ? std::move(node) : ASTNode::MakeEmptyNode();
}

ska::Parser::ASTNodePtr ska::Parser::optstatement(const Token& mustNotBe) {
	auto node = ASTNodePtr {};
	if (!m_input.expect(mustNotBe)) {
		node = statement();
	}

	return node != nullptr ? std::move(node) : ASTNode::MakeEmptyNode();
}

