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
#include "Event/ScriptLinkTokenEvent.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::MatcherImport)

ska::ASTNodePtr ska::MatcherImport::matchImport(Script& input) {
	SLOG(ska::LogLevel::Info) << "import expression";
	
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::IMPORT>());

	auto importNodeClass = input.match(TokenType::STRING);
	auto importClassName = importNodeClass.name() + ".miniska";

	auto scriptLinkNode = ASTFactory::MakeNode<Operator::SCRIPT_LINK>(ASTFactory::MakeLogicalNode(Token{ importClassName, TokenType::STRING }, ASTFactory::MakeEmptyNode()));
	auto scriptLinkEvent = ScriptLinkTokenEvent{ *scriptLinkNode, importClassName, input };
	m_parser.observable_priority_queue<ScriptLinkTokenEvent>::notifyObservers(scriptLinkEvent);

	return scriptLinkNode->type() != ExpressionType::VOID ? std::move(scriptLinkNode) : matchNewImport(input, importNodeClass);
}

ska::ASTNodePtr ska::MatcherImport::matchExport(Script& input) {
	input.match(m_reservedKeywordsPool.pattern<TokenGrammar::EXPORT>());
	if (!input.expect(m_reservedKeywordsPool.pattern<TokenGrammar::VARIABLE>())) {
		throw std::runtime_error("only a variable can be exported");
	}
	return ASTFactory::MakeNode<Operator::EXPORT>(input.statement(m_parser));
}

ska::ASTNodePtr ska::MatcherImport::matchNewImport(Script& input, const Token& importNodeClass) {
	auto importClassNameFile = importNodeClass.name() + ".miniska";
	auto scriptFile = std::ifstream{ importClassNameFile };
	if (scriptFile.fail()) {
		throw std::runtime_error("unable to find script named " + importClassNameFile);
	}

	auto nodeBlock = ASTFactory::MakeNode<Operator::BLOCK>();
	auto startEvent = BlockTokenEvent{ *nodeBlock, BlockTokenEventType::START };
	m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(startEvent);

	auto script = input.subParse(m_parser, importClassNameFile, scriptFile);

	auto importNode = ASTFactory::MakeImportNode(
		std::move(script), 
		ASTFactory::MakeLogicalNode(std::move(importNodeClass)));
	auto importEvent = ImportTokenEvent{ *importNode, input };
	m_parser.observable_priority_queue<ImportTokenEvent>::notifyObservers(importEvent);

	auto endEvent = BlockTokenEvent{ *importNode, BlockTokenEventType::END };
	m_parser.observable_priority_queue<BlockTokenEvent>::notifyObservers(endEvent);
	return importNode;
}
