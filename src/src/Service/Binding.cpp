#include <functional>
#include "Service/ReservedKeywordsPool.h"
#include "Interpreter/Interpreter.h"
#include "Config/LoggerConfigLang.h"
#include "Event/BridgeTokenEvent.h"
#include "Service/SymbolTable.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "ASTFactory.h"
#include "Binding.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Binding)

ska::Binding::Binding(Interpreter& interpreter, SymbolTable& symbolTable, TypeBuilder& typeBuilder, const ReservedKeywordsPool& reserved) :
	m_observer(symbolTable),
	m_interpreter(interpreter),
    m_reserved(reserved),
	m_typeBuilder(typeBuilder) {
	Observable<VarTokenEvent>::addObserver(m_observer);
	Observable<FunctionTokenEvent>::addObserver(m_observer);
	Observable<BlockTokenEvent>::addObserver(m_observer);
	Observable<VarTokenEvent>::addObserver(m_typeBuilder);
	Observable<FunctionTokenEvent>::addObserver(m_typeBuilder);
}

ska::Binding::~Binding() {
	Observable<BlockTokenEvent>::removeObserver(m_observer);
	Observable<FunctionTokenEvent>::removeObserver(m_observer);
	Observable<VarTokenEvent>::removeObserver(m_observer);
	Observable<FunctionTokenEvent>::removeObserver(m_typeBuilder);
	Observable<VarTokenEvent>::removeObserver(m_typeBuilder);
}

/*
ska::ASTNode& ska::Binding::bindToScript(const std::string& scriptName) {
	auto importNameNode = ASTFactory::MakeLogicalNode(Token{ scriptName, TokenType::IDENTIFIER });
	auto startEvent = BlockTokenEvent{ *importNameNode, BlockTokenEventType::START };
	Observable<BlockTokenEvent>::notifyObservers(startEvent);
}
*/

void ska::Binding::bindSymbol(const std::string& functionName, std::vector<std::string> typeNames) {
	auto functionNameToken = Token{ functionName, TokenType::IDENTIFIER };

	auto importNameNode = ASTFactory::MakeLogicalNode(Token{ "binding.miniska", TokenType::IDENTIFIER });
	auto startEvent = BlockTokenEvent{ *importNameNode, BlockTokenEventType::START };
	Observable<BlockTokenEvent>::notifyObservers(startEvent);

	auto functionNameNode = ASTFactory::MakeLogicalNode(functionNameToken);
	auto declarationEvent = FunctionTokenEvent{ *functionNameNode, FunctionTokenEventType::DECLARATION_NAME, functionNameToken.name() };
	Observable<FunctionTokenEvent>::notifyObservers(declarationEvent);

	auto parameters = std::vector<ASTNodePtr>{};
	auto index = 0u;
	auto ss = std::stringstream{};
	for (auto&& t : typeNames) {
		ss << index++;
		
		auto parameter = ASTFactory::MakeNode<Operator::PARAMETER_DECLARATION>(
			Token{ ss.str(), TokenType::IDENTIFIER },
			ASTFactory::MakeLogicalNode(m_reserved.pool.at(t).token));
		auto event = VarTokenEvent::MakeParameter(*parameter, (*parameter)[0]);
        Observable<VarTokenEvent>::notifyObservers(event);
		//parameter->buildType(m_observer);
		parameters.push_back(std::move(parameter));
		ss.clear();
	}

	auto bridgeFunction = ASTFactory::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(functionNameToken, std::move(parameters));

	auto functionEvent = VarTokenEvent::MakeFunction(*bridgeFunction);
	Observable<VarTokenEvent>::notifyObservers(functionEvent);

	auto functionBodyNode = ASTFactory::MakeNode<Operator::BLOCK>();
	auto functionDeclarationNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionNameToken, std::move(bridgeFunction), std::move(functionBodyNode));

	auto statementEvent = FunctionTokenEvent{ *functionDeclarationNode, FunctionTokenEventType::DECLARATION_STATEMENT, functionNameToken.name() };
	Observable<FunctionTokenEvent>::notifyObservers(statementEvent);
	
	m_bridges.push_back(ASTFactory::MakeNode<Operator::BRIDGE>(std::move(functionDeclarationNode)));

	auto endEvent = BlockTokenEvent{ *m_bridges.back(), BlockTokenEventType::END };
	Observable<BlockTokenEvent>::notifyObservers(endEvent);
}
