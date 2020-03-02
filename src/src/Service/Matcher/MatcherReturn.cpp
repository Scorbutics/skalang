#include "Config/LoggerConfigLang.h"
#include "MatcherReturn.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherReturn)

ska::ASTNodePtr ska::MatcherReturn::match(ScriptAST& input) {
    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::RETURN>());

    auto returnNode = ASTNodePtr {};
    auto returnStartEvent = ReturnTokenEvent { input };
    m_parser.observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnStartEvent);

    if(input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::OBJECT_BLOCK_BEGIN>())) {
        auto returnFieldNodes = std::vector<ASTNodePtr>{};
        input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::OBJECT_BLOCK_BEGIN>());
        while(!input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::OBJECT_BLOCK_END>())) {
            input.reader().mightMatch(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());

            auto field = input.reader().match(TokenType::IDENTIFIER);
            input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::AFFECTATION>());
            auto fieldValue = input.expr(m_parser);

            SLOG(ska::LogLevel::Info) << "Constructor with field \"" << field << "\" and field value \"" << (*fieldValue) << "\"";

            auto fieldNode = ASTFactory::MakeNode<Operator::VARIABLE_AFFECTATION>(std::move(field), std::move(fieldValue));

            auto event = VarTokenEvent::template Make<VarTokenEventType::VARIABLE_AFFECTATION> (*fieldNode, input);
			m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);

            returnFieldNodes.push_back(std::move(fieldNode));

            if(input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>())) {
                input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
            } else if(!input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::OBJECT_BLOCK_END>())) {
                throw std::runtime_error("an object block-end token was expected");
            }
        }

        input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::OBJECT_BLOCK_END>());

        returnNode = ASTFactory::MakeNode<Operator::RETURN>(ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(returnFieldNodes)));
        auto returnEndEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::OBJECT> (*returnNode, input);
		m_parser.observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnEndEvent);
    } else {
        returnNode = ASTFactory::MakeNode<Operator::RETURN>(input.expr(m_parser));
        auto returnEndEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::BUILTIN> (*returnNode, input);
		m_parser.observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnEndEvent);
    }
    input.reader().mightMatch(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    
    return returnNode;
}
