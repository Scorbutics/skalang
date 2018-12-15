#include "Config/LoggerConfigLang.h"
#include "MatcherIfElse.h"

#include "NodeValue/AST.h"
#include "Service/Parser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::MatcherIfElse)

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
            ifNode = ASTNode::MakeNode<Operator::IF_ELSE>(std::move(conditionExpression), std::move(conditionStatement), std::move(elseBlockStatement));
        } else {
            ifNode = ASTNode::MakeNode<Operator::IF>(std::move(conditionExpression), std::move(conditionStatement));
        }
    }

    auto event = IfElseTokenEvent {*ifNode};
	m_parser.Observable<IfElseTokenEvent>::notifyObservers(event);
    return ifNode;
}
