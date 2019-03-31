#include "Config/LoggerConfigLang.h"
#include "MatcherIfElse.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherIfElse)

ska::ASTNodePtr ska::MatcherIfElse::match(Script& input) {
    auto ifNode = ASTNodePtr{};

    input.match(m_reservedKeywordsPool.pattern<TokenGrammar::IF>());
    input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());
    
    {
        auto conditionExpression = input.expr(m_parser);
        input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

        auto conditionStatement = input.statement(m_parser);

        const auto elseToken = m_reservedKeywordsPool.pattern<TokenGrammar::ELSE>();
        if (input.expect(elseToken)) {
            input.match(elseToken);
            auto elseBlockStatement = input.statement(m_parser);
            ifNode = ASTFactory::MakeNode<Operator::IF_ELSE>(std::move(conditionExpression), std::move(conditionStatement), std::move(elseBlockStatement));
        } else {
            ifNode = ASTFactory::MakeNode<Operator::IF>(std::move(conditionExpression), std::move(conditionStatement));
        }
    }

    auto event = IfElseTokenEvent {*ifNode};
	m_parser.observable_priority_queue<IfElseTokenEvent>::notifyObservers(event);
    return ifNode;
}
