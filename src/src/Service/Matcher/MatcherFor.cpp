#include "Config/LoggerConfigLang.h"
#include "MatcherFor.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherFor)

ska::ASTNodePtr ska::MatcherFor::match(Script& input) {
    auto emptyNode = ASTFactory::MakeEmptyNode();
    auto blockEventBegin = BlockTokenEvent { *emptyNode, BlockTokenEventType::START };
	m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(blockEventBegin);

    input.match(m_reservedKeywordsPool.pattern<TokenGrammar::FOR>());
    input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_BEGIN>());

	SLOG(ska::LogLevel::Info) << "1st for loop expression (= statement)";

    auto forNodeFirstExpression = input.optstatement(m_parser);

	SLOG(ska::LogLevel::Info) << "2nd for loop expression";

    auto forNodeMidExpression = input.optexpr(m_parser, m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());

	SLOG(ska::LogLevel::Info) << "3rd for loop expression";

    auto forNodeLastExpression = input.optexpr(m_parser, m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());
    input.match(m_reservedKeywordsPool.pattern<TokenGrammar::PARENTHESIS_END>());

    auto forNodeStatement = input.optstatement(m_parser);
	SLOG(ska::LogLevel::Info) << "end for loop statement";

    auto forNode = ASTFactory::MakeNode<Operator::FOR_LOOP>(
        std::move(forNodeFirstExpression), 
        std::move(forNodeMidExpression), 
        std::move(forNodeLastExpression), 
        std::move(forNodeStatement) 
    );
    
    auto event = ForTokenEvent {*forNode};
	m_parser.observable_priority_queue<ForTokenEvent>::notifyObservers(event);
    
    auto blockEventEnd = BlockTokenEvent { *emptyNode, BlockTokenEventType::END };
	m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(blockEventEnd);
	return forNode;
}
