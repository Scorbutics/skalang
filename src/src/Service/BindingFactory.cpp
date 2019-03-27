#include <functional>
#include "Service/ReservedKeywordsPool.h"
#include "Service/Script.h"
#include "Config/LoggerConfigLang.h"
#include "Service/SymbolTable.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/SymbolTableTypeUpdater.h"
#include "ASTFactory.h"
#include "BindingFactory.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::BindingFactory)

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
}

void ska::BindingFactory::unlisten(SymbolTable& symbolTable) {
	observable_priority_queue<ScriptLinkTokenEvent>::removeObserver(symbolTable);
	observable_priority_queue<BlockTokenEvent>::removeObserver(symbolTable);
	observable_priority_queue<FunctionTokenEvent>::removeObserver(symbolTable);
	observable_priority_queue<VarTokenEvent>::removeObserver(symbolTable);
}

ska::BindingFactory::~BindingFactory() {
	observable_priority_queue<VarTokenEvent>::removeObserver(m_symbolTypeUpdater);

	observable_priority_queue<ScriptLinkTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<FunctionTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<VarTokenEvent>::removeObserver(m_typeBuilder);
}

ska::ASTNodePtr ska::BindingFactory::bindSymbol(Script& script, const std::string& functionName, std::vector<std::string> typeNames) {
	listen(script.symbols());

	//Build the function
	auto functionNameToken = Token{ functionName, TokenType::IDENTIFIER };

	auto nodeBlock = ASTFactory::MakeNode<Operator::BLOCK>();
	auto startEvent = BlockTokenEvent{ *nodeBlock, BlockTokenEventType::START };
	observable_priority_queue<BlockTokenEvent>::notifyObservers(startEvent);

	auto functionNameNode = ASTFactory::MakeLogicalNode(functionNameToken);
	auto declarationEvent = FunctionTokenEvent{ *functionNameNode, FunctionTokenEventType::DECLARATION_NAME, script, functionNameToken.name() };
	observable_priority_queue<FunctionTokenEvent>::notifyObservers(declarationEvent);

	auto parameters = std::vector<ASTNodePtr>{};
	auto index = 0u;
	auto ss = std::stringstream{};
	for (auto index = 0u; index < typeNames.size(); index++) {
		ss << index;
		auto t = m_reserved.pool.at(typeNames[index]).token;
		if (index == typeNames.size() - 1) {
			parameters.push_back(ASTFactory::MakeLogicalNode(std::move(t)));
		} else {
			auto parameter = ASTFactory::MakeNode<Operator::PARAMETER_DECLARATION>(
				Token{ ss.str(), TokenType::IDENTIFIER },
				ASTFactory::MakeLogicalNode(std::move(t)));
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
	
	auto endEvent = BlockTokenEvent{ *functionDeclarationNode, BlockTokenEventType::END };
	observable_priority_queue<BlockTokenEvent>::notifyObservers(endEvent);

	unlisten(script.symbols());

	return functionDeclarationNode;
}
