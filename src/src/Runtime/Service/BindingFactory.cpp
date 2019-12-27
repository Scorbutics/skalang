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
#include "Runtime/Value/BridgeImport.h"

#include "Service/ScriptNameBuilder.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::BindingFactory)

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

/*

ska::ASTNodePtr ska::BindingFactory::bind(BridgeImport& context) {
		// Handle the sub constructor case
	auto constructor = context.constructor();
	const auto* functionToBind = constructor[functionName];
	if (functionToBind == nullptr) {
		auto ss = std::stringstream {};
		ss << "unable to find function \"" << functionName << "\" in constructor to bind in script " << context->script->name();
		ss << " (constructor type is \"" << constructor << "\")";
		throw std::runtime_error(ss.str());
	}
	SLOG(LogLevel::Info) << " %15cRegistering constructor : " << constructor.symbol()->getName();
	return nullptr;
}

*/

ska::ASTNodePtr ska::BindingFactory::makeFunctionParameterOrReturnType(ScriptAST& script, ASTNodePtr nodeType, std::size_t parameterIndex, std::size_t totalParameters) {
	if (totalParameters == parameterIndex) {
		return nodeType;
	}

	auto ss = std::stringstream{};
	ss << parameterIndex;
	auto parameter = ASTFactory::MakeNode<Operator::PARAMETER_DECLARATION>(
				Token{ ss.str(), TokenType::IDENTIFIER, Cursor{ parameterIndex, static_cast<ColumnIndex>(parameterIndex), static_cast<LineIndex>(1) } },
				std::move(nodeType));
	auto event = VarTokenEvent::MakeParameter(*parameter, (*parameter)[0], script);
	observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	return parameter;
}

std::vector<ska::ASTNodePtr> ska::BindingFactory::makeFunctionInputOutput(ScriptAST& script, const std::vector<std::string>& typeNames) {
	auto parametersAndReturn = std::vector<ASTNodePtr>{};
	for (std::size_t index = 0u; index < typeNames.size(); index++) {
		auto tokens = ska::Tokenizer{m_reserved, typeNames[index] }.tokenize();
		auto reader = ska::TokenReader{std::move(tokens )};
		auto t = m_matcherType.match(reader);
		parametersAndReturn.push_back(std::move(makeFunctionParameterOrReturnType(script, std::move(t), index, typeNames.size() - 1)));
	}
	return parametersAndReturn;
}

std::vector<ska::ASTNodePtr> ska::BindingFactory::makeFunctionInputOutput(ScriptAST& script, const Type& fullTypeFunction) {
	auto parametersAndReturn = std::vector<ASTNodePtr>{};
	for (std::size_t index = 0u; index < fullTypeFunction.compound().size(); index++) {
		auto& type = fullTypeFunction.compound()[index];
		auto isReturnType = index == fullTypeFunction.compound().size() - 1;
		auto t = m_matcherType.match(type);
		parametersAndReturn.push_back(std::move(makeFunctionParameterOrReturnType(script, std::move(t), index, fullTypeFunction.compound().size() - 1)));
	}
	return parametersAndReturn;
}

ska::ASTNodePtr ska::BindingFactory::makeFunctionName(ScriptAST& script, const std::string& name) {
	auto functionNameNode = ASTFactory::MakeLogicalNode(Token{name, TokenType::IDENTIFIER, {}});
	auto event = FunctionTokenEvent{ *functionNameNode, FunctionTokenEventType::DECLARATION_NAME, script, name };
	observable_priority_queue<FunctionTokenEvent>::notifyObservers(event);
	return functionNameNode;
}

ska::ASTNodePtr ska::BindingFactory::makeFunctionPrototype(ScriptAST& script, ASTNodePtr nameNode, std::vector<ASTNodePtr> parametersAndReturn) {
	auto functionPrototype = ASTFactory::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(Token { nameNode->name(), nameNode->tokenType(), {} }, std::move(parametersAndReturn));
	auto event = VarTokenEvent::MakeFunction(*functionPrototype, script);
	observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	return functionPrototype;
}

ska::ASTNodePtr ska::BindingFactory::bindFunction(ScriptAST& script, const Type& fullTypeFunction) {
	const auto* functionSymbol = fullTypeFunction.symbol();
	assert(functionSymbol != nullptr && !functionSymbol->getName().empty());

	auto functionNameNode = makeFunctionName(script, functionSymbol->getName());
	auto parametersAndReturn = makeFunctionInputOutput(script, fullTypeFunction);
	auto functionPrototype = makeFunctionPrototype(script, std::move(functionNameNode), std::move(parametersAndReturn));
	return makeFunctionDeclaration(script, functionSymbol->getName(), std::move(functionPrototype));
}

ska::ASTNodePtr ska::BindingFactory::makeFunctionDeclaration(ScriptAST& script, const std::string& functionName, ASTNodePtr prototype) {
	auto functionNameToken = Token { functionName, TokenType::IDENTIFIER, {} };
	auto functionDeclarationNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionNameToken, std::move(prototype), ASTFactory::MakeNode<Operator::BLOCK>());
	auto event = FunctionTokenEvent{ *functionDeclarationNode, FunctionTokenEventType::DECLARATION_STATEMENT, script, functionNameToken.name() };
	observable_priority_queue<FunctionTokenEvent>::notifyObservers(event);
	return functionDeclarationNode;
}

ska::ASTNodePtr ska::BindingFactory::bindFunction(ScriptAST& script, const std::string& functionName, std::vector<std::string> typeNames) {
	auto lock = BindingFactorySymbolTableLock{*this, script.symbols() };

	auto functionNameNode = makeFunctionName(script, functionName);
	auto parametersAndReturn = makeFunctionInputOutput(script, typeNames);
	auto functionPrototype = makeFunctionPrototype(script, std::move(functionNameNode), std::move(parametersAndReturn));
	return makeFunctionDeclaration(script, functionName, std::move(functionPrototype));
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
