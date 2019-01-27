#include <fstream>
#include "Config/LoggerConfigLang.h"
#include "MatcherImport.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"

#include "Event/BlockTokenEvent.h"
#include "Event/ImportTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherImport)

ska::ASTNodePtr ska::MatcherImport::matchImport() {
	SLOG(ska::LogLevel::Info) << "import expression";
	
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::IMPORT>());

	//With this grammar, no other way than reading previously to retrieve the function name.
	auto importedVarName = m_input.readPrevious(3);

	auto importNodeClass = m_input.match(TokenType::STRING);
	//TODO : cache

	auto importClassName = importNodeClass.name() + ".miniska";
	auto script = std::ifstream{ importClassName };
	if (script.fail()) {
		throw std::runtime_error("unable to find script named " + importClassName);
	}

	auto importNameNode = ASTFactory::MakeLogicalNode(std::move(importNodeClass));
	auto startEvent = BlockTokenEvent{ *importNameNode, BlockTokenEventType::START };
	m_parser.Observable<BlockTokenEvent>::notifyObservers(startEvent);

	auto scriptNodeContent = m_parser.subParse(script);
	auto exportFields = std::vector<ASTNodePtr>{};
	auto hiddenFields = std::vector<ASTNodePtr>{};
	auto allFields = std::vector<ASTNodePtr>{};
	for (auto& node : (*scriptNodeContent)) {
		if (node->op() == Operator::EXPORT) {
			exportFields.push_back(ASTFactory::MakeLogicalNode(ska::Token{ (*node)[0].name(), (*node)[0].tokenType() }));
			allFields.push_back(node->stealChild(0));
		} else {
			if (OperatorTraits::isNamed(node->op())) {
				hiddenFields.push_back(ASTFactory::MakeLogicalNode(ska::Token{ (*node).name(), (*node).tokenType() }));
			}
			allFields.push_back(std::move(node));
		}
	}
	
	auto importNode = ASTFactory::MakeNode<Operator::IMPORT>(
			ASTFactory::MakeLogicalNode(importedVarName),
			std::move(importNameNode), 
			ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(exportFields)),
			ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(hiddenFields)),
			ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(allFields)));

	auto importEvent = ImportTokenEvent{ *importNode };
	m_parser.Observable<ImportTokenEvent>::notifyObservers(importEvent);

	auto endEvent = BlockTokenEvent{ *importNode, BlockTokenEventType::END };
	m_parser.Observable<BlockTokenEvent>::notifyObservers(endEvent);

	return importNode;
}

ska::ASTNodePtr ska::MatcherImport::matchExport() {
	m_input.match(m_reservedKeywordsPool.pattern<TokenGrammar::EXPORT>());
	if (!m_input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::VARIABLE>())) {
		throw std::runtime_error("only a variable can be exported");
	}
	return ASTFactory::MakeNode<Operator::EXPORT>(m_parser.statement());
}