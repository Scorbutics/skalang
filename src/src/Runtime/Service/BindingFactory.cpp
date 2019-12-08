#include <functional>
#include <fstream>
#include "Config/LoggerConfigLang.h"
#include "Service/Tokenizer.h"
#include "Service/ReservedKeywordsPool.h"
#include "Config/LoggerConfigLang.h"
#include "Service/SymbolTable.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/SymbolTableUpdater.h"
#include "Service/ASTFactory.h"
#include "BindingFactory.h"
#include "Service/Matcher/MatcherImport.h"
#include "NodeValue/ScriptAST.h"

#include "Service/ScriptNameBuilder.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::BindingFactory)

ska::BindingFactory::BindingFactory(TypeBuilder& typeBuilder, SymbolTableUpdater& symbolTypeUpdater, const ReservedKeywordsPool& reserved) :
	m_reserved(reserved),
	m_typeBuilder(typeBuilder),
	m_symbolTypeUpdater(symbolTypeUpdater),
	m_matcherType(reserved) {
	observable_priority_queue<VarTokenEvent>::addObserver(m_typeBuilder);
	observable_priority_queue<FunctionTokenEvent>::addObserver(m_typeBuilder);
	observable_priority_queue<ScriptLinkTokenEvent>::addObserver(m_typeBuilder);

	observable_priority_queue<FunctionTokenEvent>::addObserver(m_symbolTypeUpdater);
	observable_priority_queue<VarTokenEvent>::addObserver(m_symbolTypeUpdater);
}

void ska::BindingFactory::internalListen(SymbolTable& symbolTable) {
	observable_priority_queue<VarTokenEvent>::addObserver(symbolTable);
	observable_priority_queue<FunctionTokenEvent>::addObserver(symbolTable);
	observable_priority_queue<BlockTokenEvent>::addObserver(symbolTable);
	observable_priority_queue<ScriptLinkTokenEvent>::addObserver(symbolTable);
	observable_priority_queue<ImportTokenEvent>::addObserver(symbolTable);
}

void ska::BindingFactory::internalUnlisten(SymbolTable& symbolTable) {
	observable_priority_queue<ScriptLinkTokenEvent>::removeObserver(symbolTable);
	observable_priority_queue<BlockTokenEvent>::removeObserver(symbolTable);
	observable_priority_queue<FunctionTokenEvent>::removeObserver(symbolTable);
	observable_priority_queue<VarTokenEvent>::removeObserver(symbolTable);
	observable_priority_queue<ImportTokenEvent>::removeObserver(symbolTable);
}

ska::BindingFactory::~BindingFactory() {
	observable_priority_queue<VarTokenEvent>::removeObserver(m_symbolTypeUpdater);
	observable_priority_queue<FunctionTokenEvent>::removeObserver(m_symbolTypeUpdater);

	observable_priority_queue<ScriptLinkTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<FunctionTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<VarTokenEvent>::removeObserver(m_typeBuilder);
}

ska::ASTNodePtr ska::BindingFactory::createImport(StatementParser& parser, ska::ScriptAST& input, Token scriptPathToken) {
	return MatcherImport::createNewImport(parser, *this, *this, input, std::move(scriptPathToken));
}

ska::BridgeImportRaw ska::BindingFactory::import(StatementParser& parser, ScriptAST& parentScript, std::pair<std::string, std::string> import) {
	auto lock = BindingFactorySymbolTableLock{ *this, parentScript.symbols() };

	auto importClassNameFile = ScriptNameDeduce(parentScript.name(), import.second);
	auto scriptLinkNode = ASTFactory::MakeNode<Operator::SCRIPT_LINK>(ASTFactory::MakeLogicalNode(Token{ importClassNameFile, TokenType::STRING, {} }, ASTFactory::MakeEmptyNode()));
	auto scriptLinkEvent = ScriptLinkTokenEvent{ *scriptLinkNode, importClassNameFile, parentScript };
	observable_priority_queue<ScriptLinkTokenEvent>::notifyObservers(scriptLinkEvent);

	auto varNode = ASTNodePtr{};
	if (scriptLinkNode->type() == ExpressionType::VOID) {
		auto importNode = createImport(parser, parentScript, Token{ importClassNameFile, TokenType::STRING, {} });
		varNode = ASTFactory::MakeNode<Operator::VARIABLE_DECLARATION>(Token{ std::move(import.first), TokenType::IDENTIFIER, {} }, std::move(importNode));
	} else {
		varNode = ASTFactory::MakeNode<Operator::VARIABLE_DECLARATION>(Token{ std::move(import.first), TokenType::IDENTIFIER, {} }, std::move(scriptLinkNode));
	}

	auto event = VarTokenEvent::template Make<VarTokenEventType::VARIABLE_DECLARATION>(*varNode, parentScript);
	observable_priority_queue<VarTokenEvent>::notifyObservers(event);

	lock.release();

	auto boundScript = parentScript.useImport(importClassNameFile);
	assert(boundScript != nullptr);
	return { std::move(varNode), boundScript->handle() };
}

ska::ASTNodePtr ska::BindingFactory::bindSymbol(ScriptAST& script, const std::string& functionName, std::vector<std::string> typeNames) {
	auto lock = BindingFactorySymbolTableLock{*this, script.symbols() };

	//Build the function
	auto functionNameToken = Token{ functionName, TokenType::IDENTIFIER, {} };

	auto functionNameNode = ASTFactory::MakeLogicalNode(functionNameToken);
	auto declarationEvent = FunctionTokenEvent{ *functionNameNode, FunctionTokenEventType::DECLARATION_NAME, script, functionNameToken.name() };
	observable_priority_queue<FunctionTokenEvent>::notifyObservers(declarationEvent);

	auto parameters = std::vector<ASTNodePtr>{};
	auto ss = std::stringstream{};
	for (std::size_t index = 0u; index < typeNames.size(); index++) {
		ss << index;
		auto tokens = ska::Tokenizer{m_reserved, typeNames[index] }.tokenize();
		auto reader = ska::TokenReader{std::move(tokens )};
		auto t = m_matcherType.match(reader);
		if (index == typeNames.size() - 1) {
			parameters.push_back(std::move(t));
		} else {
			auto parameter = ASTFactory::MakeNode<Operator::PARAMETER_DECLARATION>(
				Token{ ss.str(), TokenType::IDENTIFIER, Cursor{ index, static_cast<ColumnIndex>(index), static_cast<LineIndex>(1) } },
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

	return functionDeclarationNode;
}


ska::BindingFactorySymbolTableLock::BindingFactorySymbolTableLock(BindingFactory& factory, SymbolTable& table) :
	m_factory(factory),
	m_symbolTable(table) {
	m_factory.internalListen(m_symbolTable);
}

ska::BindingFactorySymbolTableLock::BindingFactorySymbolTableLock(BindingFactorySymbolTableLock&& factoryLock) noexcept :
	m_factory(factoryLock.m_factory),
	m_symbolTable(factoryLock.m_symbolTable),
	m_freed(factoryLock.m_freed){
	factoryLock.m_freed = true;
}

ska::BindingFactorySymbolTableLock::~BindingFactorySymbolTableLock() {
	release();
}

void ska::BindingFactorySymbolTableLock::release() {
	if (!m_freed) {
		m_factory.internalUnlisten(m_symbolTable);
		m_freed = true;
	}
}
