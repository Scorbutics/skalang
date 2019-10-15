#include "Config/LoggerConfigLang.h"
#include "MatcherBlock.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"
#include "Service/ASTFactory.h"
#include "NodeValue/ScriptAST.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherBlock)

ska::ASTNodePtr ska::MatcherBlock::match(ScriptAST& input, const std::string& content) {
	if (content == m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>().name()) {

		SLOG(ska::LogLevel::Info) << "block start detected";

		input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_BEGIN>());
		
		auto blockNode = ASTFactory::MakeNode<Operator::BLOCK>();
		auto startEvent = BlockTokenEvent { *blockNode, BlockTokenEventType::START };
		m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(startEvent);

        auto blockNodeStatements = std::vector<ASTNodePtr>{};
		while (!input.reader().expect(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>())) {
			auto optionalStatement = input.optstatement(m_parser, Token{});
			if (!optionalStatement->logicalEmpty()) {
				blockNodeStatements.push_back(std::move(optionalStatement));
			} else {
				break;
			}
		}

		blockNode = ASTFactory::MakeNode<Operator::BLOCK>(std::move(blockNodeStatements));

		input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>());

		SLOG(ska::LogLevel::Info) << "block end";

		auto endEvent = BlockTokenEvent { *blockNode, BlockTokenEventType::END };
		m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(endEvent);
		return blockNode;
	} else if (content == m_reservedKeywordsPool.pattern<TokenGrammar::BLOCK_END>().name()) {
		throw std::runtime_error("syntax error : Block end token encountered when not expected");
	} else {
		auto expression = input.expr(m_parser);
		input.reader().match(m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
		return expression;
	}

	input.optexpr(m_parser, m_reservedKeywordsPool.pattern<TokenGrammar::STATEMENT_END>());
	return nullptr;
}
