#include <iterator>
#include <fstream>
#include "Matcher/MatcherBlock.h"
#include "Config/LoggerConfigLang.h"
#include "NodeValue/AST.h"
#include "NodeValue/Operator.h"
#include "Service/StatementParser.h"
#include "ReservedKeywordsPool.h"

#include "Service/Tokenizer.h"
#include "Service/TokenReader.h"
#include "Service/ASTFactory.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::StatementParser)

ska::StatementParser::StatementParser(const ReservedKeywordsPool& reservedKeywordsPool, TokenReader& input) :
	m_input(input),
	m_reservedKeywordsPool(reservedKeywordsPool),
	m_expressionParser(reservedKeywordsPool, *this, m_input),
	m_matcherBlock(m_input, m_reservedKeywordsPool, *this),
	m_matcherFor(m_input, m_reservedKeywordsPool, *this),
	m_matcherIfElse(m_input, m_reservedKeywordsPool, *this),
	m_matcherVar(m_input, m_reservedKeywordsPool, *this),
	m_matcherReturn(m_input, m_reservedKeywordsPool, *this),
	m_matcherImport(m_input, m_reservedKeywordsPool, *this) {
}

ska::StatementParser::ASTNodePtr ska::StatementParser::parse() {
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
	return ASTFactory::MakeNode<Operator::BLOCK>(std::move(blockNodeStatements));
}

ska::StatementParser::ASTNodePtr ska::StatementParser::statement() {
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

ska::StatementParser::ASTNodePtr ska::StatementParser::matchExpressionStatement() {
	SLOG(ska::LogLevel::Info) << "Expression-statement found";

    auto expressionResult = expr();
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    if(expressionResult == nullptr) {
		SLOG(ska::LogLevel::Info) << "NOP statement";
		return nullptr;
    }
    return expressionResult;
}

ska::StatementParser::ASTNodePtr ska::StatementParser::matchReservedKeyword(const std::size_t keywordIndex) {
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
	
	case static_cast<std::size_t>(TokenGrammar::EXPORT) :
		return m_matcherImport.matchExport();

	default: {
			std::stringstream ss;
			ss << (keywordIndex < static_cast<std::size_t>(TokenGrammar::UNUSED_Last_Length) ? TokenGrammarSTR[keywordIndex] : "UNKNOWN TYPE" );
			error("Unhandled keyword type : " + ss.str());
			return nullptr;
		}
	}
}

ska::StatementParser::ASTNodePtr ska::StatementParser::expr() {
	return m_expressionParser.parse();
}

void ska::StatementParser::error(const std::string& message) {
	throw std::runtime_error("syntax error : " + message);
}

ska::StatementParser::ASTNodePtr ska::StatementParser::optexpr(const Token& mustNotBe) {
	auto node = ASTNodePtr {};
	if (!m_input.expect(mustNotBe)) {
		node = expr();
	}
	return node != nullptr ? std::move(node) : ASTFactory::MakeEmptyNode();
}

ska::ASTNodePtr ska::StatementParser::subParse(std::ifstream& file) {
	auto content = std::string (
		(std::istreambuf_iterator<char>(file)),
		(std::istreambuf_iterator<char>()) 
	);

	auto tokenizer = Tokenizer{ m_reservedKeywordsPool, std::move(content)};
	auto tokens = tokenizer.tokenize();
	const auto& [lastSource, lastIndex] = m_input.setSource(tokens);
	auto result = parse();
	m_input.setSource(*lastSource, lastIndex);
	return result;
}

ska::StatementParser::ASTNodePtr ska::StatementParser::optstatement(const Token& mustNotBe) {
	auto node = ASTNodePtr {};
	if (!m_input.expect(mustNotBe)) {
		node = statement();
	}

	return node != nullptr ? std::move(node) : ASTFactory::MakeEmptyNode();
}

