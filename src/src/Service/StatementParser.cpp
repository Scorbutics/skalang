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
#include "Service/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::StatementParser)

ska::StatementParser::StatementParser(const ReservedKeywordsPool& reservedKeywordsPool) :
	m_reservedKeywordsPool(reservedKeywordsPool),
	m_expressionParser(reservedKeywordsPool, *this),
	m_matcherBlock(m_reservedKeywordsPool, *this),
	m_matcherFor(m_reservedKeywordsPool, *this),
	m_matcherIfElse(m_reservedKeywordsPool, *this),
	m_matcherVar(m_reservedKeywordsPool, *this),
	m_matcherReturn(m_reservedKeywordsPool, *this),
	m_matcherImport(m_reservedKeywordsPool, *this) {
}

ska::StatementParser::ASTNodePtr ska::StatementParser::parse(Script& input) {
  if(input.empty()) {
		return nullptr;
	}

	auto blockNodeStatements = std::vector<ASTNodePtr>{};
    while (!input.empty()) {
		auto optionalStatement = optstatement(input);
		if (optionalStatement != nullptr && !optionalStatement->logicalEmpty()) {
			blockNodeStatements.push_back(std::move(optionalStatement));
		} else {
			break;
		}
	}
	return ASTFactory::MakeNode<Operator::BLOCK>(std::move(blockNodeStatements));
}

ska::StatementParser::ASTNodePtr ska::StatementParser::statement(Script& input) {
	if (input.empty()) {
		return nullptr;
	}

	const auto token = input.actual();
	switch (token.type()) {
	case TokenType::RESERVED:
		return matchReservedKeyword(input, std::get<std::size_t>(token.content()));

	case TokenType::RANGE:
		return m_matcherBlock.match(input, std::get<std::string>(token.content()));

	default:
        return matchExpressionStatement(input);
	}
}

ska::StatementParser::ASTNodePtr ska::StatementParser::matchExpressionStatement(Script& input) {
	SLOG(ska::LogLevel::Info) << "Expression-statement found";

    auto expressionResult = expr(input);
    input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    if(expressionResult == nullptr) {
			SLOG(ska::LogLevel::Info) << "NOP statement";
			return nullptr;
    }
    return expressionResult;
}

ska::StatementParser::ASTNodePtr ska::StatementParser::matchReservedKeyword(Script& input, const std::size_t keywordIndex) {
	switch (keywordIndex) {
	case static_cast<std::size_t>(TokenGrammar::FOR):
		return m_matcherFor.match(input);

	case static_cast<std::size_t>(TokenGrammar::IF):
		return m_matcherIfElse.match(input);

	case static_cast<std::size_t>(TokenGrammar::VARIABLE):
		return m_matcherVar.matchDeclaration(input);

	case static_cast<std::size_t>(TokenGrammar::FUNCTION):
		return expr(input);

    case static_cast<std::size_t>(TokenGrammar::RETURN):
        return m_matcherReturn.match(input);
	
	case static_cast<std::size_t>(TokenGrammar::EXPORT) :
		return m_matcherImport.matchExport(input);

	default: {
			std::stringstream ss;
			ss << (keywordIndex < static_cast<std::size_t>(TokenGrammar::UNUSED_Last_Length) ? TokenGrammarSTR[keywordIndex] : "UNKNOWN TYPE" );
			error("Unhandled keyword type : " + ss.str());
			return nullptr;
		}
	}
}

ska::StatementParser::ASTNodePtr ska::StatementParser::expr(Script& input) {
	return m_expressionParser.parse(input);
}

void ska::StatementParser::error(const std::string& message) {
	throw std::runtime_error("syntax error : " + message);
}

ska::StatementParser::ASTNodePtr ska::StatementParser::optexpr(Script& input, const Token& mustNotBe) {
	auto node = ASTNodePtr {};
	if (!input.expect(mustNotBe)) {
		node = expr(input);
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
	auto script = Script { tokens };
	return parse(script);
}

ska::StatementParser::ASTNodePtr ska::StatementParser::optstatement(Script& input, const Token& mustNotBe) {
	auto node = ASTNodePtr {};
	if (!input.expect(mustNotBe)) {
		node = statement(input);
	}

	return node != nullptr ? std::move(node) : ASTFactory::MakeEmptyNode();
}

