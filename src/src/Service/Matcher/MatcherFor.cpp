#include "Config/LoggerConfigLang.h"
#include "MatcherFor.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherFor)

ska::ASTNodePtr ska::MatcherFor::match() {
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::FOR>());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

	SLOG(ska::LogLevel::Info) << "1st for loop expression (= statement)";

    auto forNodeFirstExpression = m_parser.optstatement();

	SLOG(ska::LogLevel::Info) << "2nd for loop expression";

    auto forNodeMidExpression = m_parser.optexpr(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());

	SLOG(ska::LogLevel::Info) << "3rd for loop expression";

    auto forNodeLastExpression = m_parser.optexpr(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

    auto forNodeStatement = m_parser.statement();
	SLOG(ska::LogLevel::Info) << "end for loop statement";

    auto forNode = ASTFactory::MakeNode<Operator::FOR_LOOP>(std::move(forNodeFirstExpression), std::move(forNodeMidExpression), std::move(forNodeLastExpression), std::move(forNodeStatement));
    
    auto event = ForTokenEvent {*forNode};
	m_parser.Observable<ForTokenEvent>::notifyObservers(event);
	return forNode;
}
