#include "Config/LoggerConfigLang.h"
#include "MatcherVar.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherVar)

ska::ASTNodePtr ska::MatcherVar::matchDeclaration(ScriptAST& input) {
	SLOG(ska::LogLevel::Info) << "variable declaration";

	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::VARIABLE>());
	auto varNodeIdentifier = input.reader().match(TokenType::IDENTIFIER);
    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::AFFECTATION>());

	SLOG(ska::LogLevel::Info) << "equal sign matched, reading expression";

    auto varNodeExpression = input.expr(m_parser);

    input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());

	SLOG(ska::LogLevel::Info) << "expression end with symbol ;";

    auto varNode = ASTFactory::MakeNode<Operator::VARIABLE_DECLARATION>(std::move(varNodeIdentifier), std::move(varNodeExpression));
    
    auto event = VarTokenEvent::template Make<VarTokenEventType::VARIABLE_DECLARATION> (*varNode, input);
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);

    return varNode;
}

ska::ASTNodePtr ska::MatcherVar::matchAffectation(ScriptAST& input, ASTNodePtr varAffectedNode) {
	input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::AFFECTATION>());
	auto expressionNode = input.expr(m_parser);
	if (expressionNode == nullptr) {
		throw std::runtime_error("syntax error : Affectation incomplete : no expression");
	}

	auto affectationNode = ASTFactory::MakeNode<Operator::VARIABLE_AFFECTATION>(std::move(varAffectedNode), std::move(expressionNode));
	auto event = VarTokenEvent::template Make<VarTokenEventType::AFFECTATION>(*affectationNode, input);
	m_parser.observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	return affectationNode;
}
