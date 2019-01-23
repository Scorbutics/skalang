#include "Config/LoggerConfigLang.h"
#include "MatcherIfElse.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherIfElse)

ska::ASTNodePtr ska::MatcherIfElse::match() {
    auto ifNode = ASTNodePtr{};

    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::IF>());
    m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());
    
    {
        auto conditionExpression = m_parser.expr();
        m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

        auto conditionStatement = m_parser.statement();

        const auto elseToken = m_reservedKeywordsPool.pattern<TokenGrammar::ELSE>();
        if (m_input.expect(elseToken)) {
            m_input.match(elseToken);
            auto elseBlockStatement = m_parser.statement();
            ifNode = ASTFactory::MakeNode<Operator::IF_ELSE>(std::move(conditionExpression), std::move(conditionStatement), std::move(elseBlockStatement));
        } else {
            ifNode = ASTFactory::MakeNode<Operator::IF>(std::move(conditionExpression), std::move(conditionStatement));
        }
    }

    auto event = IfElseTokenEvent {*ifNode};
	m_parser.Observable<IfElseTokenEvent>::notifyObservers(event);
    return ifNode;
}
