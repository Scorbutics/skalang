#include <fstream>
#include "Config/LoggerConfigLang.h"
#include "MatcherImport.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "Service/Script.h"

#include "Event/BlockTokenEvent.h"
#include "Event/ImportTokenEvent.h"
#include "Event/BridgeTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherImport)

ska::ASTNodePtr ska::MatcherImport::matchImport(Script& input) {
	SLOG(ska::LogLevel::Info) << "import expression";
	
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::IMPORT>());

	//With this grammar, no other way than reading previously to retrieve the function name.
	auto importedVarName = input.readPrevious(3);

	auto importNodeClass = input.match(TokenType::STRING);
	//TODO : cache
	auto importClassName = importNodeClass.name() + ".miniska";

	auto bridgeNode = ASTFactory::MakeNode<Operator::BRIDGE>(ASTFactory::MakeLogicalNode(Token{ importClassName, TokenType::STRING }, ASTFactory::MakeEmptyNode()));
	auto bridgeEvent = BridgeTokenEvent{ *bridgeNode };
	m_parser.observable_priority_queue<BridgeTokenEvent>::notifyObservers(bridgeEvent);


	return bridgeNode->type() != ExpressionType::VOID ? std::move(bridgeNode) : matchNewImport(input, importedVarName, importNodeClass);
}

ska::ASTNodePtr ska::MatcherImport::matchExport(Script& input) {
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::EXPORT>());
	if (!input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::VARIABLE>())) {
		throw std::runtime_error("only a variable can be exported");
	}
	return ASTFactory::MakeNode<Operator::EXPORT>(input.statement(m_parser));
}

ska::ASTNodePtr ska::MatcherImport::matchNewImport(Script& input, const Token& importedVarName, const Token& importNodeClass) {
	auto importClassName = importNodeClass.name() + ".miniska";
	auto scriptFile = std::ifstream{ importClassName };
	if (scriptFile.fail()) {
		//TODO handle bridging
		throw std::runtime_error("unable to find script named " + importClassName);
	}

	auto nodeBlock = ASTFactory::MakeNode<Operator::BLOCK>();
	auto startEvent = BlockTokenEvent{ *nodeBlock, BlockTokenEventType::START };
	m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(startEvent);

	auto script = input.subParse(m_parser, importClassName, scriptFile);
	auto scriptNodeContent = m_parser.parse(script);
	auto exportFields = std::vector<ASTNodePtr>{};
	auto hiddenFields = std::vector<ASTNodePtr>{};
	auto allFields = std::vector<ASTNodePtr>{};
	if (scriptNodeContent == nullptr) {
		throw std::runtime_error("unable to parse script named " + importClassName);
	}
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
		std::move(ASTFactory::MakeLogicalNode(std::move(importNodeClass))),
		ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(exportFields)),
		ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(hiddenFields)),
		ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(allFields)),
		ASTFactory::MakeLogicalNode(Token {importClassName, TokenType::STRING}));

	auto importEvent = ImportTokenEvent{ *importNode };
	m_parser.observable_priority_queue<ImportTokenEvent>::notifyObservers(importEvent);

	auto endEvent = BlockTokenEvent{ *importNode, BlockTokenEventType::END };
	m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(endEvent);
	return importNode;
}
