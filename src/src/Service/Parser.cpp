#include <iostream>
#include "Matcher/MatcherBlock.h"
#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "NodeValue/Operator.h"
#include "Service/Parser.h"
#include "ReservedKeywordsPool.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Parser)

ska::Parser::Parser(const ReservedKeywordsPool& reservedKeywordsPool, TokenReader& input) :
	m_input(input),
	m_reservedKeywordsPool(reservedKeywordsPool),
	m_shuntingYardParser(reservedKeywordsPool, *this, m_input),
	m_matcherBlock(m_input, m_reservedKeywordsPool, *this),
	m_matcherFor(m_input, m_reservedKeywordsPool, *this),
	m_matcherIfElse(m_input, m_reservedKeywordsPool, *this),
	m_matcherVar(m_input, m_reservedKeywordsPool, *this),
	m_matcherReturn(m_input, m_reservedKeywordsPool, *this) {
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
		return m_matcherBlock.match(std::get<std::string>(token.content()));

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

ska::Parser::ASTNodePtr ska::Parser::matchReservedKeyword(const std::size_t keywordIndex) {
	switch (keywordIndex) {
	case static_cast<std::size_t>(TokenGrammar::FOR):
		return m_matcherFor.match();

	case static_cast<std::size_t>(TokenGrammar::IF):
		return m_matcherIfElse.match();

	case static_cast<std::size_t>(TokenGrammar::VARIABLE):
		return m_matcherVar.matchDeclaration();

	case static_cast<std::size_t>(TokenGrammar::FUNCTION):
		return expr();

    case static_cast<std::size_t>(TokenGrammar::RETURN):
        return m_matcherReturn.match();

	default: {
			std::stringstream ss;
			ss << (keywordIndex < static_cast<std::size_t>(TokenGrammar::UNUSED_Last_Length) ? TokenGrammarSTR[keywordIndex] : "UNKNOWN TYPE" );
			error("Unhandled keyword type : " + ss.str());
			return nullptr;
		}
	}
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

