#include "Config/LoggerConfigLang.h"
#include "MatcherVar.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherVar)

ska::ASTNodePtr ska::MatcherVar::matchDeclaration() {
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

ska::ASTNodePtr ska::MatcherVar::matchAffectation(ASTNodePtr varAffectedNode) {
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::AFFECTATION>());
	auto expressionNode = m_parser.expr();
	if (expressionNode == nullptr) {
		throw std::runtime_error("syntax error : Affectation incomplete : no expression");
	}

	auto affectationNode = ASTNode::MakeNode<Operator::VARIABLE_AFFECTATION>(std::move(varAffectedNode), std::move(expressionNode));
	auto event = VarTokenEvent::template Make<VarTokenEventType::AFFECTATION>(*affectationNode);
	m_parser.Observable<VarTokenEvent>::notifyObservers(event);
	return affectationNode;
}
