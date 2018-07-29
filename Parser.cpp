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

	std::string* content = nullptr;
	const auto token = m_input.actual();
	switch (token.type()) {
	case TokenType::RESERVED:
		return matchReservedKeyword(std::get<std::size_t>(token.content()));
		break;

	case TokenType::RANGE:
		return matchBlock(std::get<std::string>(token.content()));
		break;

	default:
		{
			std::cout << "expression found" << std::endl;
			auto expressionResult = expr();
			m_input.match(Token{ ";", TokenType::SYMBOL });
			if(expressionResult == nullptr) {
				std::cout << "null expression" << std::endl;
				return nullptr;
			} 
			return std::move(expressionResult);
		}
		break;
	}
	return nullptr;
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
	} 
			
	optexpr(Token{ ";", TokenType::SYMBOL });	
	return nullptr;
}

ska::Parser::ASTNodePtr ska::Parser::matchReservedKeyword(const std::size_t keywordIndex) {
	auto reservedKeywordNode = ASTNodePtr{};

	switch (keywordIndex) {

	case ReservedKeywords::FOR:
		reservedKeywordNode = std::make_unique<ska::ASTNode>(Operator::FOR_LOOP);
		m_input.match(Token{ ReservedKeywords::FOR, TokenType::RESERVED });
		m_input.match(Token{ "(", TokenType::RANGE });

		std::cout << "1st for loop expression (= statement)" << std::endl;
		reservedKeywordNode->add(optstatement());

		std::cout << "2nd for loop expression" << std::endl;
		reservedKeywordNode->add(optexpr(Token{ ";", TokenType::SYMBOL }));
		m_input.match(Token{ ";", TokenType::SYMBOL });

		std::cout << "3rd for loop expression" << std::endl;
		reservedKeywordNode->add(optexpr(Token{ ")", TokenType::RANGE }));
		m_input.match(Token{ ")", TokenType::RANGE });

		reservedKeywordNode->add(statement());
		std::cout << "end for loop statement" << std::endl;
		break;

	case ReservedKeywords::ELSE:
		//TODO delete
		//???
		break;
	case ReservedKeywords::IF: {
		reservedKeywordNode = std::make_unique<ska::ASTNode>(Operator::IF);

		m_input.match(Token{ ReservedKeywords::IF, TokenType::RESERVED });
		m_input.match(Token{ "(", TokenType::RANGE });

		reservedKeywordNode->add(expr());
		m_input.match(Token{ ")", TokenType::RANGE });

		reservedKeywordNode->add(statement());

		const auto elseToken = Token{ ReservedKeywords::ELSE, TokenType::RESERVED };
		if (m_input.expect(elseToken)) {
			reservedKeywordNode->op = Operator::IF_ELSE;
			m_input.match(elseToken);
			reservedKeywordNode->add(statement());
		}
	}
	break;

	case ReservedKeywords::VAR: {
		reservedKeywordNode = std::make_unique<ASTNode>(Operator::VARIABLE_DECLARATION);
		std::cout << "variable declaration" << std::endl;
		m_input.match(Token{ ReservedKeywords::VAR, TokenType::RESERVED });
		const auto& identifier = m_input.match(TokenType::IDENTIFIER);
		reservedKeywordNode->add(std::make_unique<ska::ASTNode>(identifier));

		m_input.match(Token{ "=", TokenType::SYMBOL });
		std::cout << "equal sign matched, reading expression" << std::endl;
		reservedKeywordNode->add(expr());

		m_input.match(Token{ ";", TokenType::SYMBOL });
		std::cout << "expression end with symbol ;" << std::endl;
					
		//m_currentScope->registerIdentifier(std::get<std::string>(identifier.content()), std::move(value));
	}
	break;

	case ReservedKeywords::FUNCTION: 
		return expr();
				
	default:
		error();
	}
	return reservedKeywordNode;
}

ska::Parser::ASTNodePtr ska::Parser::expr() {
	return m_shuntingYardParser.parse();
}

void ska::Parser::error() {
	throw std::runtime_error("syntax error");
}


ska::Parser::ASTNodePtr ska::Parser::optexpr(const Token& mustNotBe) {
	if (!m_input.expect(mustNotBe)) {
		return expr();
	}
	return nullptr;
}

ska::Parser::ASTNodePtr ska::Parser::optstatement(const Token& mustNotBe) {
	if (!m_input.expect(mustNotBe)) {
		return statement();
	} 
	m_input.match(Token{ ";", TokenType::SYMBOL });
	return nullptr;
}
