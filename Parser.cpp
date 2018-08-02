#include <iostream>
#include "Parser.h"
#include "ReservedKeywordsPool.h"

ska::Parser::Parser(const ReservedKeywordsPool& reservedKeywordsPool, TokenReader& input) :
	m_input(input),
	m_reservedKeywordsPool(reservedKeywordsPool),
	m_shuntingYardParser(reservedKeywordsPool, *this, m_input) {
}

std::pair<ska::Parser::ASTNodePtr, ska::Scope> ska::Parser::parse() {
	auto scope = ska::Scope { nullptr };
	return std::make_pair(statement(), std::move(scope));
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
    std::cout << "Expression-statement found" << std::endl;

    auto expressionResult = expr();
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    if(expressionResult == nullptr) {
        std::cout << "NOP statement" << std::endl;
        return nullptr;
    }
    return expressionResult;
}

ska::Parser::ASTNodePtr ska::Parser::matchBlock(const std::string& content) {
	auto blockNode = std::make_unique<ska::ASTNode>(Operator::BLOCK);
	if (content == m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>().asString()) {
		std::cout << "block start detected" << std::endl;

		m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());
		do {
			auto optionalStatement = optstatement();
			if (optionalStatement != nullptr) {
				blockNode->add(std::move(optionalStatement));
			}
		} while (!m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>()));
		m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());

		std::cout << "block end" << std::endl;
		auto event = BlockTokenEvent {*blockNode};
		Observable<BlockTokenEvent>::notifyObservers(event);
		return blockNode;
	} else {
	    error();
	}

	optexpr(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
	return nullptr;
}

ska::Parser::ASTNodePtr ska::Parser::matchForKeyword() {
    auto forNode = std::make_unique<ska::ASTNode>(Operator::FOR_LOOP);
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::FOR>());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

    std::cout << "1st for loop expression (= statement)" << std::endl;
    forNode->add(optstatement());

    std::cout << "2nd for loop expression" << std::endl;
    forNode->add(optexpr(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>()));
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());

    std::cout << "3rd for loop expression" << std::endl;
    forNode->add(optexpr(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>()));
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

    forNode->addIfExists(statement());

    std::cout << "end for loop statement" << std::endl;

    auto event = ForTokenEvent {*forNode};
    Observable<ForTokenEvent>::notifyObservers(event);
    return forNode;
}

ska::Parser::ASTNodePtr ska::Parser::matchIfOrIfElseKeyword() {
    auto ifNode = std::make_unique<ska::ASTNode>(Operator::IF);

    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::IF>());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

    ifNode->add(expr());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

    ifNode->add(statement());

    const auto elseToken = m_reservedKeywordsPool.pattern<TokenGrammar::ELSE>();
    if (m_input.expect(elseToken)) {
        ifNode->op = Operator::IF_ELSE;
        m_input.match(elseToken);
        ifNode->add(statement());
    }
    auto event = IfElseTokenEvent {*ifNode};
    Observable<IfElseTokenEvent>::notifyObservers(event);
    return ifNode;
}

ska::Parser::ASTNodePtr ska::Parser::matchVarKeyword() {
    auto varNode = std::make_unique<ASTNode>(Operator::VARIABLE_DECLARATION);
    std::cout << "variable declaration" << std::endl;

    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::VARIABLE>());
    const auto& identifier = m_input.match(TokenType::IDENTIFIER);
    varNode->add(std::make_unique<ska::ASTNode>(identifier));

    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::AFFECTATION>());
    std::cout << "equal sign matched, reading expression" << std::endl;
    varNode->add(expr());

    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    std::cout << "expression end with symbol ;" << std::endl;

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

	default:
		error();
		return nullptr;
	}
}

ska::Parser::ASTNodePtr ska::Parser::expr() {
	return m_shuntingYardParser.parse();
}

void ska::Parser::error() {
	throw std::runtime_error("syntax error");
}

ska::Parser::ASTNodePtr ska::Parser::optexpr(const Token& mustNotBe) {
	auto node = ASTNodePtr {};
	if (!m_input.expect(mustNotBe)) {
		node = expr();
	}
	return node != nullptr ? std::move(node) : std::make_unique<ASTNode>(Token{});
}

ska::Parser::ASTNodePtr ska::Parser::optstatement(const Token& mustNotBe) {
	auto node = ASTNodePtr {};
	if (!m_input.expect(mustNotBe)) {
		node = statement();
	}

	return node != nullptr ? std::move(node) : std::make_unique<ASTNode>(Token{});
}
