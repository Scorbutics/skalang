#include <functional>
#include <fstream>
#include "Config/LoggerConfigLang.h"
#include "Service/Tokenizer.h"
#include "Service/ReservedKeywordsPool.h"
#include "Interpreter/Value/Script.h"
#include "Config/LoggerConfigLang.h"
#include "Service/SymbolTable.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/SymbolTableTypeUpdater.h"
#include "ASTFactory.h"
#include "BindingFactory.h"
#include "Matcher/MatcherImport.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::BindingFactory)

ska::BindingFactory::BindingFactory(TypeBuilder& typeBuilder, SymbolTableTypeUpdater& symbolTypeUpdater, const ReservedKeywordsPool& reserved) :
    m_reserved(reserved),
	m_typeBuilder(typeBuilder),
	m_symbolTypeUpdater(symbolTypeUpdater) {
	observable_priority_queue<VarTokenEvent>::addObserver(m_typeBuilder);
	observable_priority_queue<FunctionTokenEvent>::addObserver(m_typeBuilder);
	observable_priority_queue<ScriptLinkTokenEvent>::addObserver(m_typeBuilder);
	
	observable_priority_queue<VarTokenEvent>::addObserver(m_symbolTypeUpdater);
}

void ska::BindingFactory::listen(SymbolTable& symbolTable) {
	observable_priority_queue<VarTokenEvent>::addObserver(symbolTable);
	observable_priority_queue<FunctionTokenEvent>::addObserver(symbolTable);
	observable_priority_queue<BlockTokenEvent>::addObserver(symbolTable);
	observable_priority_queue<ScriptLinkTokenEvent>::addObserver(symbolTable);
	observable_priority_queue<ImportTokenEvent>::addObserver(symbolTable);
}

void ska::BindingFactory::unlisten(SymbolTable& symbolTable) {
	observable_priority_queue<ScriptLinkTokenEvent>::removeObserver(symbolTable);
	observable_priority_queue<BlockTokenEvent>::removeObserver(symbolTable);
	observable_priority_queue<FunctionTokenEvent>::removeObserver(symbolTable);
	observable_priority_queue<VarTokenEvent>::removeObserver(symbolTable);
	observable_priority_queue<ImportTokenEvent>::removeObserver(symbolTable);
}

ska::BindingFactory::~BindingFactory() {
	observable_priority_queue<VarTokenEvent>::removeObserver(m_symbolTypeUpdater);

	observable_priority_queue<ScriptLinkTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<FunctionTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<VarTokenEvent>::removeObserver(m_typeBuilder);
}

ska::ASTNodePtr ska::BindingFactory::getNodeFromTypeName(const std::string& typeName) {
	if (m_reserved.pool.find(typeName) != m_reserved.pool.end()) {
		SLOG(LogLevel::Debug) << "type for node found in reserved pool : " << typeName;
		return ASTFactory::MakeLogicalNode(m_reserved.pool.at(typeName).token);
	}

	const auto& typeDelimiterToken = m_reserved.pattern<TokenGrammar::TYPE_DELIMITER>();
	const auto splitSymbol = typeName.find_last_of(std::get<std::string>(typeDelimiterToken.content()));
	if (splitSymbol != std::string::npos) {
		assert(splitSymbol > 0);
		//Handles script namespace
		auto typeNamespaceToken = Token{ typeName.substr(0, splitSymbol - 1), TokenType::IDENTIFIER };
		auto typeFieldToken = Token{ typeName.substr(splitSymbol + 1), TokenType::IDENTIFIER };

		SLOG(LogLevel::Debug) << "type for node using namespace class " << std::get<std::string>(typeNamespaceToken.content()) << " with class " << std::get<std::string>(typeFieldToken.content());

		return ASTFactory::MakeLogicalNode(std::move(typeNamespaceToken), ASTFactory::MakeLogicalNode(std::move(typeFieldToken)));
	}

	SLOG(LogLevel::Debug) << "type for node as identifier " << typeName;

	return ASTFactory::MakeLogicalNode(Token{ typeName, TokenType::IDENTIFIER });
}

ska::ASTNodePtr ska::BindingFactory::createImport(StatementParser& parser, ska::Script& input, Token scriptPathToken) {
	return MatcherImport::createNewImport(parser, *this, *this, input, std::move(scriptPathToken));
}

ska::ASTNodePtr ska::BindingFactory::import(StatementParser& parser, Script& script, std::vector<std::pair<std::string, std::string>> imports) {
	listen(script.symbols());
	auto result = std::vector<ASTNodePtr> {};
	for (const auto& scriptImporter : imports) {
		auto importClassNameFile = scriptImporter.second + ".miniska";	
		auto scriptLinkNode = ASTFactory::MakeNode<Operator::SCRIPT_LINK>(ASTFactory::MakeLogicalNode(Token{ importClassNameFile, TokenType::STRING }, ASTFactory::MakeEmptyNode()));
		auto scriptLinkEvent = ScriptLinkTokenEvent{ *scriptLinkNode, importClassNameFile, script };
		observable_priority_queue<ScriptLinkTokenEvent>::notifyObservers(scriptLinkEvent);

		auto varNode = ASTNodePtr{};
		if (scriptLinkNode->type() == ExpressionType::VOID) {
			auto importNode = createImport(parser, script, Token{ std::move(scriptImporter.second), TokenType::STRING });
			varNode = ASTFactory::MakeNode<Operator::VARIABLE_DECLARATION>(Token{ std::move(scriptImporter.first), TokenType::IDENTIFIER }, std::move(importNode));
		} else {
			varNode = ASTFactory::MakeNode<Operator::VARIABLE_DECLARATION>(Token{ std::move(scriptImporter.first), TokenType::IDENTIFIER }, std::move(scriptLinkNode));
		}

		auto event = VarTokenEvent::template Make<VarTokenEventType::VARIABLE_DECLARATION>(*varNode, script);
		observable_priority_queue<VarTokenEvent>::notifyObservers(event);
		result.emplace_back(std::move(varNode));
	}
	unlisten(script.symbols());
	return ASTFactory::MakeNode<Operator::BLOCK>(std::move(result));
}

ska::ASTNodePtr ska::BindingFactory::bindSymbol(Script& script, const std::string& functionName, std::vector<std::string> typeNames) {
	listen(script.symbols());

	//Build the function
	auto functionNameToken = Token{ functionName, TokenType::IDENTIFIER };

	auto functionNameNode = ASTFactory::MakeLogicalNode(functionNameToken);
	auto declarationEvent = FunctionTokenEvent{ *functionNameNode, FunctionTokenEventType::DECLARATION_NAME, script, functionNameToken.name() };
	observable_priority_queue<FunctionTokenEvent>::notifyObservers(declarationEvent);

	auto parameters = std::vector<ASTNodePtr>{};
	auto ss = std::stringstream{};
	for (std::size_t index = 0u; index < typeNames.size(); index++) {
		ss << index;
		auto t = getNodeFromTypeName(typeNames[index]);
		if (index == typeNames.size() - 1) {
			parameters.push_back(std::move(t));
		} else {
			auto parameter = ASTFactory::MakeNode<Operator::PARAMETER_DECLARATION>(
				Token{ ss.str(), TokenType::IDENTIFIER },
				std::move(t));
			auto event = VarTokenEvent::MakeParameter(*parameter, (*parameter)[0], script);
			observable_priority_queue<VarTokenEvent>::notifyObservers(event);
			parameters.push_back(std::move(parameter));
		}		
		ss.clear();
	}

	auto bridgeFunction = ASTFactory::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(functionNameToken, std::move(parameters));

	auto functionEvent = VarTokenEvent::MakeFunction(*bridgeFunction, script);
	observable_priority_queue<VarTokenEvent>::notifyObservers(functionEvent);

	auto functionDeclarationNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionNameToken, std::move(bridgeFunction), ASTFactory::MakeNode<Operator::BLOCK>());

	auto statementEvent = FunctionTokenEvent{ *functionDeclarationNode, FunctionTokenEventType::DECLARATION_STATEMENT, script, functionNameToken.name() };
	observable_priority_queue<FunctionTokenEvent>::notifyObservers(statementEvent);

	unlisten(script.symbols());
	return functionDeclarationNode;
}
