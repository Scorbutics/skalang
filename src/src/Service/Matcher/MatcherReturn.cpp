#include "Config/LoggerConfigLang.h"
#include "MatcherReturn.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherReturn)

ska::ASTNodePtr ska::MatcherReturn::match(TokenReader& input) {
    input.match(m_reservedKeywordsPool.pattern<TokenGrammar::RETURN>());

    auto returnNode = ASTNodePtr {};
    auto returnStartEvent = ReturnTokenEvent {};
    m_parser.Observable<ReturnTokenEvent>::notifyObservers(returnStartEvent);

    if(input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>())) {
        auto returnFieldNodes = std::vector<ASTNodePtr>{};
        input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());
        while(!input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>())) {
            auto field = input.match(TokenType::IDENTIFIER);
            input.match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
            auto fieldValue = m_parser.expr(input);

            const std::string name = "???";

            SLOG(ska::LogLevel::Info) << "Constructor " << name << " with field \"" << field << "\" and field value \"" << (*fieldValue) << "\"";

            auto fieldNode = ASTFactory::MakeNode<Operator::VARIABLE_DECLARATION>(std::move(field), std::move(fieldValue));

            auto event = VarTokenEvent::template Make<VarTokenEventType::VARIABLE_DECLARATION> (*fieldNode);
			m_parser.Observable<VarTokenEvent>::notifyObservers(event);

            returnFieldNodes.push_back(std::move(fieldNode));
            
            if (input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>())) {
                input.match(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
            }
        }
        
        input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());
    
        returnNode = ASTFactory::MakeNode<Operator::RETURN>(ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(returnFieldNodes)));
        auto returnEndEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::OBJECT> (*returnNode);
		m_parser.Observable<ReturnTokenEvent>::notifyObservers(returnEndEvent);
    } else {
        returnNode = ASTFactory::MakeNode<Operator::RETURN>(m_parser.expr(input));
        auto returnEndEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::BUILTIN> (*returnNode);
		m_parser.Observable<ReturnTokenEvent>::notifyObservers(returnEndEvent);
    }
    input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    
    return returnNode;
}
