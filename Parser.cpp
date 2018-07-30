#include <iostream>
#include "Parser.h"
#include "ReservedKeywordsPool.h"

ska::Parser::Parser(TokenReader& input) :
	m_input(input),
	m_shuntingYardParser(*this, m_input) {
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
    m_input.match(Token{ ";", TokenType::SYMBOL });
    if(expressionResult == nullptr) {
        std::cout << "NOP statement" << std::endl;
        return nullptr;
    }
    return expressionResult;
}

ska::Parser::ASTNodePtr ska::Parser::matchBlock(const std::string& content) {
	auto blockNode = std::make_unique<ska::ASTNode>(Operator::BLOCK);
	if (content[0] == '{' ) {
		std::cout << "block start detected" << std::endl;

		m_input.match(Token{ "{", TokenType::RANGE });
		do {
			auto optionalStatement = optstatement();
			if (optionalStatement != nullptr) {
				blockNode->add(std::move(optionalStatement));
			}
		} while (!m_input.expect(Token{ "}", TokenType::RANGE }));
		m_input.match(Token{ "}", TokenType::RANGE });

		std::cout << "block end" << std::endl;
		return blockNode;
	} else {
	    error();
	}

	optexpr(Token{ ";", TokenType::SYMBOL });
	return nullptr;
}

ska::Parser::ASTNodePtr ska::Parser::matchForKeyword() {
    auto forNode = std::make_unique<ska::ASTNode>(Operator::FOR_LOOP);
    m_input.match(Token{ ReservedKeywords::FOR, TokenType::RESERVED });
    m_input.match(Token{ "(", TokenType::RANGE });

    std::cout << "1st for loop expression (= statement)" << std::endl;
    forNode->add(optstatement());

    std::cout << "2nd for loop expression" << std::endl;
    forNode->add(optexpr(Token{ ";", TokenType::SYMBOL }));
    m_input.match(Token{ ";", TokenType::SYMBOL });

    std::cout << "3rd for loop expression" << std::endl;
    forNode->add(optexpr(Token{ ")", TokenType::RANGE }));
    m_input.match(Token{ ")", TokenType::RANGE });

    forNode->addIfExists(statement());

    std::cout << "end for loop statement" << std::endl;

    return forNode;
}

ska::Parser::ASTNodePtr ska::Parser::matchIfOrIfElseKeyword() {
    auto ifNode = std::make_unique<ska::ASTNode>(Operator::IF);

    m_input.match(Token{ ReservedKeywords::IF, TokenType::RESERVED });
    m_input.match(Token{ "(", TokenType::RANGE });

    ifNode->add(expr());
    m_input.match(Token{ ")", TokenType::RANGE });

    ifNode->add(statement());

    const auto elseToken = Token{ ReservedKeywords::ELSE, TokenType::RESERVED };
    if (m_input.expect(elseToken)) {
        ifNode->op = Operator::IF_ELSE;
        m_input.match(elseToken);
        ifNode->add(statement());
    }
    return ifNode;
}

ska::Parser::ASTNodePtr ska::Parser::matchVarKeyword() {
    auto varNode = std::make_unique<ASTNode>(Operator::VARIABLE_DECLARATION);
    std::cout << "variable declaration" << std::endl;
    m_input.match(Token{ ReservedKeywords::VAR, TokenType::RESERVED });
    const auto& identifier = m_input.match(TokenType::IDENTIFIER);
    varNode->add(std::make_unique<ska::ASTNode>(identifier));

    m_input.match(Token{ "=", TokenType::SYMBOL });
    std::cout << "equal sign matched, reading expression" << std::endl;
    varNode->add(expr());

    m_input.match(Token{ ";", TokenType::SYMBOL });
    std::cout << "expression end with symbol ;" << std::endl;

    //m_currentScope->registerIdentifier(std::get<std::string>(identifier.content()), std::move(value));
    return varNode;
}

ska::Parser::ASTNodePtr ska::Parser::matchReservedKeyword(const std::size_t keywordIndex) {
	switch (keywordIndex) {
	case ReservedKeywords::FOR:
		return matchForKeyword();

	case ReservedKeywords::IF:
		return matchIfOrIfElseKeyword();

	case ReservedKeywords::VAR:
		return matchVarKeyword();

	case ReservedKeywords::FUNCTION:
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
	m_input.match(Token{ ";", TokenType::SYMBOL });
	return node != nullptr ? std::move(node) : std::make_unique<ASTNode>(Token{});
}
