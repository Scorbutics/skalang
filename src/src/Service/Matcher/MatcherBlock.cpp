#include "Config/LoggerConfigLang.h"
#include "MatcherBlock.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherBlock)

ska::ASTNodePtr ska::MatcherBlock::match(const std::string& content) {
	if (content == m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>().name()) {

		SLOG(ska::LogLevel::Info) << "block start detected";

		m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());
		
		auto blockNode = ASTNode::MakeNode<Operator::BLOCK>();
		auto startEvent = BlockTokenEvent { *blockNode, BlockTokenEventType::START };
		m_parser.Observable<BlockTokenEvent>::notifyObservers(startEvent);

        auto blockNodeStatements = std::vector<ASTNodePtr>{};
		while (!m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>())) {
			auto optionalStatement = m_parser.optstatement();
			if (!optionalStatement->logicalEmpty()) {
				blockNodeStatements.push_back(std::move(optionalStatement));
			} else {
				break;
			}
		}

		blockNode = ASTNode::MakeNode<Operator::BLOCK>(std::move(blockNodeStatements));

		m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());

		SLOG(ska::LogLevel::Info) << "block end";

		auto endEvent = BlockTokenEvent { *blockNode, BlockTokenEventType::END };
		m_parser.Observable<BlockTokenEvent>::notifyObservers(endEvent);
		return blockNode;
	} else if (content == m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>().name()) {
		throw std::runtime_error("syntax error : Block end token encountered when not expected");
	} else {
		return m_parser.matchExpressionStatement();
	}

	m_parser.optexpr(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
	return nullptr;
}
