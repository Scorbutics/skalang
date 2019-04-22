#include <fstream>
#include "Config/LoggerConfigLang.h"
#include "MatcherImport.h"

#include "NodeValue/AST.h"
#include "Service/StatementParser.h"
#include "Service/TokenReader.h"
#include "Service/ReservedKeywordsPool.h"
#include "Service/ASTFactory.h"
#include "Interpreter/Value/Script.h"

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
		throw std::runtime_error("only a variable declaration can be exported");
	}
	return ASTFactory::MakeNode<Operator::EXPORT>(input.statement(m_parser));
}

ska::ASTNodePtr ska::MatcherImport::matchNewImport(Script& input, Token importNodeClass) {
	return createNewImport(m_parser, m_parser, m_parser, input, importNodeClass);
}

ska::ASTNodePtr ska::MatcherImport::createNewImport(
		StatementParser& parser,
		observable_priority_queue<ska::BlockTokenEvent>& obsBlock, 
		observable_priority_queue<ska::ImportTokenEvent>& obsImport,
		Script& input, 
		Token importNodeClass) {

	auto importClassNameFile = importNodeClass.name() + ".miniska";
	auto scriptFile = std::ifstream{ importClassNameFile };
	if (scriptFile.fail()) {
		throw std::runtime_error("unable to find script named " + importClassNameFile);
	}

	auto nodeBlock = ska::ASTFactory::MakeNode<ska::Operator::BLOCK>();
	auto startEvent = ska::BlockTokenEvent{ *nodeBlock, ska::BlockTokenEventType::START };
	obsBlock.observable_priority_queue<ska::BlockTokenEvent>::notifyObservers(startEvent);

	auto script = input.subParse(parser, importClassNameFile, scriptFile);

	auto importNode = ska::ASTFactory::MakeNode<ska::Operator::IMPORT>(ska::ASTFactory::MakeLogicalNode(std::move(importNodeClass)));
	auto importEvent = ska::ImportTokenEvent{ *importNode, input };
	obsImport.observable_priority_queue<ska::ImportTokenEvent>::notifyObservers(importEvent);

	auto endEvent = ska::BlockTokenEvent{ *importNode, ska::BlockTokenEventType::END };
	obsBlock.observable_priority_queue<ska::BlockTokenEvent>::notifyObservers(endEvent);

	return importNode;
}