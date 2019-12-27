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
#include "BridgeASTBuilder.h"
#include "Service/Matcher/MatcherImport.h"
#include "NodeValue/ScriptAST.h"
#include "Runtime/Value/BridgeImport.h"

#include "Service/ScriptNameBuilder.h"

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::BridgeASTBuilder)

ska::BridgeASTBuilder::BridgeASTBuilder(TypeBuilder& typeBuilder, SymbolTableUpdater& symbolTypeUpdater, const ReservedKeywordsPool& reserved) :
	m_reserved(reserved),
	m_typeBuilder(typeBuilder),
	m_symbolTypeUpdater(symbolTypeUpdater),
	m_matcherType(reserved) {
	observable_priority_queue<VarTokenEvent>::addObserver(m_typeBuilder);
	observable_priority_queue<ReturnTokenEvent>::addObserver(m_typeBuilder);
	observable_priority_queue<FunctionTokenEvent>::addObserver(m_typeBuilder);
	observable_priority_queue<ScriptLinkTokenEvent>::addObserver(m_typeBuilder);

	observable_priority_queue<FunctionTokenEvent>::addObserver(m_symbolTypeUpdater);
	observable_priority_queue<VarTokenEvent>::addObserver(m_symbolTypeUpdater);
}

void ska::BridgeASTBuilder::internalListen(SymbolTable& symbolTable) {
	auto [ element, isInserted ] = m_symbolTableLockCounter.emplace(&symbolTable, 0);
	if (!isInserted) {
		m_symbolTableLockCounter.at(&symbolTable)++;
	} else {
		observable_priority_queue<VarTokenEvent>::addObserver(symbolTable);
		observable_priority_queue<FunctionTokenEvent>::addObserver(symbolTable);
		observable_priority_queue<BlockTokenEvent>::addObserver(symbolTable);
		observable_priority_queue<ScriptLinkTokenEvent>::addObserver(symbolTable);
		observable_priority_queue<ImportTokenEvent>::addObserver(symbolTable);
	}
}

void ska::BridgeASTBuilder::internalUnlisten(SymbolTable& symbolTable) {
	auto& counter = m_symbolTableLockCounter.at(&symbolTable);
	if (--counter <= 0) {
		observable_priority_queue<ScriptLinkTokenEvent>::removeObserver(symbolTable);
		observable_priority_queue<BlockTokenEvent>::removeObserver(symbolTable);
		observable_priority_queue<FunctionTokenEvent>::removeObserver(symbolTable);
		observable_priority_queue<VarTokenEvent>::removeObserver(symbolTable);
		observable_priority_queue<ImportTokenEvent>::removeObserver(symbolTable);
	}
}

ska::BridgeASTBuilder::~BridgeASTBuilder() {
	observable_priority_queue<VarTokenEvent>::removeObserver(m_symbolTypeUpdater);
	observable_priority_queue<FunctionTokenEvent>::removeObserver(m_symbolTypeUpdater);

	observable_priority_queue<ScriptLinkTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<FunctionTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<ReturnTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<VarTokenEvent>::removeObserver(m_typeBuilder);
}

ska::ASTNodePtr ska::BridgeASTBuilder::createImport(StatementParser& parser, ska::ScriptAST& input, Token scriptPathToken) {
	return MatcherImport::createNewImport(parser, *this, *this, input, std::move(scriptPathToken));
}

ska::BridgeImportRaw ska::BridgeASTBuilder::import(StatementParser& parser, ScriptAST& parentScript, std::pair<std::string, std::string> import) {
	auto lock = BridgeASTBuilderSymbolTableLock{ *this, parentScript.symbols() };

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

ska::ASTNodePtr ska::BridgeASTBuilder::makeFunctionParameterOrReturnType(ScriptAST& script, ASTNodePtr nodeType, std::size_t parameterIndex, std::size_t totalParameters) {
	if (totalParameters == parameterIndex) {
		return nodeType;
	}

	auto lock = BridgeASTBuilderSymbolTableLock{ *this, script.symbols() };
	auto ss = std::stringstream{};
	ss << parameterIndex;
	auto parameter = ASTFactory::MakeNode<Operator::PARAMETER_DECLARATION>(
				Token{ ss.str(), TokenType::IDENTIFIER, Cursor{ parameterIndex, static_cast<ColumnIndex>(parameterIndex), static_cast<LineIndex>(1) } },
				std::move(nodeType));
	auto event = VarTokenEvent::MakeParameter(*parameter, (*parameter)[0], script);
	observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	return parameter;
}

std::vector<ska::ASTNodePtr> ska::BridgeASTBuilder::makeFunctionInputOutput(ScriptAST& script, const std::vector<std::string>& typeNames) {
	auto parametersAndReturn = std::vector<ASTNodePtr>{};
	for (std::size_t index = 0u; index < typeNames.size(); index++) {
		auto tokens = ska::Tokenizer{m_reserved, typeNames[index] }.tokenize();
		auto reader = ska::TokenReader{std::move(tokens )};
		auto t = m_matcherType.match(reader);
		parametersAndReturn.push_back(std::move(makeFunctionParameterOrReturnType(script, std::move(t), index, typeNames.size() - 1)));
	}
	return parametersAndReturn;
}

std::vector<ska::ASTNodePtr> ska::BridgeASTBuilder::makeFunctionInputOutput(ScriptAST& script, const Type& fullTypeFunction) {
	auto parametersAndReturn = std::vector<ASTNodePtr>{};
	for (std::size_t index = 0u; index < fullTypeFunction.compound().size(); index++) {
		auto& type = fullTypeFunction.compound()[index];
		auto isReturnType = index == fullTypeFunction.compound().size() - 1;
		auto t = m_matcherType.match(type);
		parametersAndReturn.push_back(std::move(makeFunctionParameterOrReturnType(script, std::move(t), index, fullTypeFunction.compound().size() - 1)));
	}
	return parametersAndReturn;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFunctionName(ScriptAST& script, const std::string& name) {
	auto lock = BridgeASTBuilderSymbolTableLock{ *this, script.symbols() };
	auto functionNameNode = ASTFactory::MakeLogicalNode(Token{name, TokenType::IDENTIFIER, {}});
	auto event = FunctionTokenEvent{ *functionNameNode, FunctionTokenEventType::DECLARATION_NAME, script, name };
	observable_priority_queue<FunctionTokenEvent>::notifyObservers(event);
	return functionNameNode;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFunctionPrototype(ScriptAST& script, ASTNodePtr nameNode, std::vector<ASTNodePtr> parametersAndReturn) {
	auto lock = BridgeASTBuilderSymbolTableLock{ *this, script.symbols() };
	auto functionPrototype = ASTFactory::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(Token { nameNode->name(), nameNode->tokenType(), {} }, std::move(parametersAndReturn));
	auto event = VarTokenEvent::MakeFunction(*functionPrototype, script);
	observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	return functionPrototype;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeVariable(ScriptAST& script, const std::string& name, ASTNodePtr value) {
	auto lock = BridgeASTBuilderSymbolTableLock{*this, script.symbols() };
	auto tokenField = Token {name, TokenType::IDENTIFIER, {} };
	auto variable = ASTFactory::MakeNode<Operator::VARIABLE_DECLARATION>(std::move(tokenField), std::move(value));
	auto event = VarTokenEvent::template Make<VarTokenEventType::VARIABLE_DECLARATION> (*variable, script);
	observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	return variable;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeCustomObjectReturn(ScriptAST& script, std::vector<BridgeField> fieldList) {
	auto lock = BridgeASTBuilderSymbolTableLock{*this, script.symbols() };
	auto returnNode = ASTNodePtr {};
	auto returnStartEvent = ReturnTokenEvent { script };
	observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnStartEvent);

	auto returnFieldNodes = std::vector<ASTNodePtr>{};

	for(auto& field : fieldList) {
		auto fieldValue = makeFunction(script, BridgeFunctionData { field.type });
		auto fieldVariable = makeVariable(script, field.name, std::move(fieldValue));

		returnFieldNodes.push_back(std::move(fieldVariable));
	}

	returnNode = ASTFactory::MakeNode<Operator::RETURN>(ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(returnFieldNodes)));
	auto returnEndEvent = ReturnTokenEvent::template Make<ReturnTokenEventType::OBJECT> (*returnNode, script);
	observable_priority_queue<ReturnTokenEvent>::notifyObservers(returnEndEvent);
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFunctionDeclaration(ScriptAST& script, ASTNodePtr prototype, std::vector<BridgeField> fieldList) {
	auto lock = BridgeASTBuilderSymbolTableLock{*this, script.symbols() };
	const std::string& functionName = prototype->name();
	auto functionNameToken = Token { functionName, TokenType::IDENTIFIER, {} };
	auto returnNode = makeCustomObjectReturn(script, std::move(fieldList));
	auto functionDeclarationNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionNameToken, std::move(prototype), ASTFactory::MakeNode<Operator::BLOCK>());
	auto event = FunctionTokenEvent{ *functionDeclarationNode, FunctionTokenEventType::DECLARATION_STATEMENT, script, functionNameToken.name() };
	observable_priority_queue<FunctionTokenEvent>::notifyObservers(event);
	return functionDeclarationNode;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFunctionPrototype(ScriptAST& script, const Type& fullTypeFunction) {
	auto lock = BridgeASTBuilderSymbolTableLock{*this, script.symbols() };
	const auto* functionSymbol = fullTypeFunction.symbol();
	assert(functionSymbol != nullptr && !functionSymbol->getName().empty());

	auto functionNameNode = makeFunctionName(script, functionSymbol->getName());
	auto parametersAndReturn = makeFunctionInputOutput(script, fullTypeFunction);
	auto functionPrototype = makeFunctionPrototype(script, std::move(functionNameNode), std::move(parametersAndReturn));
	return functionPrototype;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFunctionPrototype(ScriptAST& script, const std::string& functionName, std::vector<std::string> typeNames) {
	auto lock = BridgeASTBuilderSymbolTableLock{*this, script.symbols() };
	auto functionNameNode = makeFunctionName(script, functionName);
	auto parametersAndReturn = makeFunctionInputOutput(script, typeNames);
	auto functionPrototype = makeFunctionPrototype(script, std::move(functionNameNode), std::move(parametersAndReturn));
	return functionPrototype;
}

ska::ASTNodePtr ska::BridgeASTBuilder::makeFunction(ScriptAST& script, BridgeFunctionData data) {
	auto prototype = ASTNodePtr{};
	if (!data.typeNames().empty()) {
		prototype = makeFunctionPrototype(script, data.name(), std::move(data.stealTypeNames()));
	} else {
		prototype = makeFunctionPrototype(script, data.type());
	}
	auto fields = data.stealFields();
	return makeFunctionDeclaration(script, std::move(prototype), std::move(fields));
}

ska::BridgeASTBuilderSymbolTableLock::BridgeASTBuilderSymbolTableLock(BridgeASTBuilder& factory, SymbolTable& table) :
	m_factory(factory),
	m_symbolTable(table) {
	m_factory.internalListen(m_symbolTable);
}

ska::BridgeASTBuilderSymbolTableLock::BridgeASTBuilderSymbolTableLock(BridgeASTBuilderSymbolTableLock&& factoryLock) noexcept :
	m_factory(factoryLock.m_factory),
	m_symbolTable(factoryLock.m_symbolTable),
	m_freed(factoryLock.m_freed){
	factoryLock.m_freed = true;
}

ska::BridgeASTBuilderSymbolTableLock::~BridgeASTBuilderSymbolTableLock() {
	release();
}

void ska::BridgeASTBuilderSymbolTableLock::release() {
	if (!m_freed) {
		m_factory.internalUnlisten(m_symbolTable);
		m_freed = true;
	}
}
