#include "LoggerConfigLang.h"
#include "MatcherVar.h"

#include "AST.h"
#include "Parser.h"
#include "TokenReader.h"
#include "ReservedKeywordsPool.h"
#include "BlockTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::MatcherVar)

ska::ASTNodePtr ska::MatcherVar::match() {
	SLOG(ska::LogLevel::Info) << "variable declaration";

	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::VARIABLE>());
	auto varNodeIdentifier = m_input.match(TokenType::IDENTIFIER);
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::AFFECTATION>());

	SLOG(ska::LogLevel::Info) << "equal sign matched, reading expression";

    auto varNodeExpression = m_parser.expr();

    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());

	SLOG(ska::LogLevel::Info) << "expression end with symbol ;";

    auto varNode = ASTNode::MakeNode<Operator::VARIABLE_DECLARATION>(std::move(varNodeIdentifier), std::move(varNodeExpression));
    
    auto event = VarTokenEvent::template Make<VarTokenEventType::VARIABLE_DECLARATION> (*varNode);
	m_parser.Observable<VarTokenEvent>::notifyObservers(event);

    return varNode;
}
