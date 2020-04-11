#include "Config/LoggerConfigLang.h"
#include "MatcherIfElse.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherIfElse)

ska::ASTNodePtr ska::MatcherIfElse::match(ScriptAST& input) {
    auto ifNode = ASTNodePtr{};

    SLOG(ska::LogLevel::Debug) << "Matching if";

    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::IF>());

    {
        auto conditionExpression = input.expr(m_parser);
        SLOG(ska::LogLevel::Debug) << "Matching if - expression done";

        auto conditionStatement = input.statement(m_parser);
        SLOG(ska::LogLevel::Debug) << "Matching if - if statement done";

        const auto elseToken = m_reservedKeywordsPool.pattern<TokenGrammar::ELSE>();
        if (input.reader().expect(elseToken)) {
            SLOG(ska::LogLevel::Debug) << "Matching if - else statement done";
            input.reader().match(elseToken);
            auto elseBlockStatement = input.statement(m_parser);
            ifNode = ASTFactory::MakeNode<Operator::IF_ELSE>(std::move(conditionExpression), std::move(conditionStatement), std::move(elseBlockStatement));
        } else {
            ifNode = ASTFactory::MakeNode<Operator::IF>(std::move(conditionExpression), std::move(conditionStatement));
        }
    }
    SLOG(ska::LogLevel::Debug) << "if matching done";
    auto event = IfElseTokenEvent {*ifNode};
	m_parser.observable_priority_queue<IfElseTokenEvent>::notifyObservers(event);
    return ifNode;
}
