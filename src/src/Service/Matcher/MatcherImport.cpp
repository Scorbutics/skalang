#include <fstream>
#include "Config/LoggerConfigLang.h"
#include "MatcherImport.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Event/BlockTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherImport)

ska::ASTNodePtr ska::MatcherImport::matchImport() {
	SLOG(ska::LogLevel::Info) << "import expression";
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::IMPORT>());
	auto importNodeClass = m_input.match(TokenType::STRING);

	//TODO : cache

	auto importClassName = importNodeClass.name() + ".minisk";
	auto script = std::ifstream{ importClassName };
	if (script.fail()) {
		throw std::runtime_error("unable to find script named " + importClassName);
	}

	auto scriptNodeContent = m_parser.subParse(script);
	auto exportFields = std::vector<ASTNodePtr>{};
	for (auto& node : (*scriptNodeContent)) {
		if (node->op() == Operator::EXPORT) {
			exportFields.push_back(node->stealChild(0));
		}
	}
	return ASTNode::MakeNode<Operator::IMPORT>(ASTNode::MakeLogicalNode(std::move(importNodeClass)), ASTNode::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(exportFields)));
}

ska::ASTNodePtr ska::MatcherImport::matchExport() {
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::EXPORT>());
	if (!m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::VARIABLE>())) {
		throw std::runtime_error("only a variable can be exported");
	}
	return ASTNode::MakeNode<Operator::EXPORT>(m_parser.statement());
}