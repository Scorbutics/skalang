#include <iostream>
#include "LoggerConfigLang.h"
#include "AST.h"
#include "Operator.h"
#include "Parser.h"
#include "ReservedKeywordsPool.h"

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
	SLOG(ska::LogLevel::Info) << "Expression-statement found";

    auto expressionResult = expr();
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    if(expressionResult == nullptr) {

		SLOG(ska::LogLevel::Info) << "NOP statement";

	return nullptr;
    }
    return expressionResult;
}

ska::Parser::ASTNodePtr ska::Parser::matchBlock(const std::string& content) {
	if (content == m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>().name()) {

		SLOG(ska::LogLevel::Info) << "block start detected";

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

		SLOG(ska::LogLevel::Info) << "block end";

		auto endEvent = BlockTokenEvent { *blockNode, BlockTokenEventType::END };
		Observable<BlockTokenEvent>::notifyObservers(endEvent);
		return blockNode;
	} else if (content == m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>().name()) {
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

	SLOG(ska::LogLevel::Info) << "1st for loop expression (= statement)";

    auto forNodeFirstExpression = optstatement();

	SLOG(ska::LogLevel::Info) << "2nd for loop expression";

    auto forNodeMidExpression = optexpr(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());

	SLOG(ska::LogLevel::Info) << "3rd for loop expression";

    auto forNodeLastExpression = optexpr(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

    auto forNodeStatement = statement();
	SLOG(ska::LogLevel::Info) << "end for loop statement";

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
	SLOG(ska::LogLevel::Info) << "variable declaration";

	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::VARIABLE>());
	auto varNodeIdentifier = m_input.match(TokenType::IDENTIFIER);
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::AFFECTATION>());

	SLOG(ska::LogLevel::Info) << "equal sign matched, reading expression";

    auto varNodeExpression = expr();

    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());

	SLOG(ska::LogLevel::Info) << "expression end with symbol ;";

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

    auto returnFieldNodes = std::vector<ASTNodePtr>{};

    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());
    while(!m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>())) {
        auto field = m_input.match(TokenType::IDENTIFIER);
        m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
        auto fieldValue = expr();

        const std::string name = "???";

        SLOG(ska::LogLevel::Info) << "Constructor " << name << " with field \"" << field << "\" and field value \"" << (*fieldValue) << "\"";

        auto fieldNode = ASTNode::MakeNode<Operator::VARIABLE_DECLARATION>(std::move(field), std::move(fieldValue));		
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

