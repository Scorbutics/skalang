#include "Config/LoggerConfigLang.h"
#include "MatcherReturn.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "Interpreter/Value/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherReturn)

ska::ASTNodePtr ska::MatcherReturn::match(Script& input) {
    input.match(m_reservedKeywordsPool.pattern<TokenGrammar::RETURN>());

    auto returnNode = ASTNodePtr {};
    auto returnStartEvent = ReturnTokenEvent { input };
    m_parser.observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnStartEvent);

    if(input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>())) {
        auto returnFieldNodes = std::vector<ASTNodePtr>{};
        input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());
        while(!input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>())) {
            auto field = input.match(TokenType::IDENTIFIER);
            input.match(m_reservedKeywordsPool.pattern<TokenGrammar::TYPE_DELIMITER>());
            auto fieldValue = input.expr(m_parser);

            const std::string name = "???";

            SLOG(ska::LogLevel::Info) << "Constructor " << name << " with field \"" << field << "\" and field value \"" << (*fieldValue) << "\"";

            auto fieldNode = ASTFactory::MakeNode<Operator::VARIABLE_DECLARATION>(std::move(field), std::move(fieldValue));

            auto event = VarTokenEvent::template Make<VarTokenEventType::VARIABLE_DECLARATION> (*fieldNode, input);
			m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);

            returnFieldNodes.push_back(std::move(fieldNode));
            
            if (input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>())) {
                input.match(m_reservedKeywordsPool.pattern<TokenGrammar::ARGUMENT_DELIMITER>());
            }
        }
        
        input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());
    
        returnNode = ASTFactory::MakeNode<Operator::RETURN>(ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(returnFieldNodes)));
        auto returnEndEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::OBJECT> (*returnNode, input);
		m_parser.observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnEndEvent);
    } else {
        returnNode = ASTFactory::MakeNode<Operator::RETURN>(input.expr(m_parser));
        auto returnEndEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::BUILTIN> (*returnNode, input);
		m_parser.observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnEndEvent);
    }
    input.match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
    
    return returnNode;
}
