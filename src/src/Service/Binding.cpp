#include <functional>
#include "Service/ReservedKeywordsPool.h"
#include "Interpreter/Interpreter.h"
#include "Config/LoggerConfigLang.h"
#include "Event/BridgeTokenEvent.h"
#include "Service/SymbolTable.h"
#include "ASTFactory.h"
#include "Binding.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Binding)

ska::Binding::Binding(Interpreter& interpreter, SymbolTable& symbolTable, const ReservedKeywordsPool& reserved) :
	m_observer(symbolTable),
	m_interpreter(interpreter),
    m_reserved(reserved) {
}

void ska::Binding::bindSymbol(const std::string& functionName, std::vector<std::string> typeNames) {
	auto functionNameToken = Token{ functionName, TokenType::IDENTIFIER };

	auto parameters = std::vector<ASTNodePtr>{};
	auto index = 0u;
	auto ss = std::stringstream{};
	for (auto&& t : typeNames) {
		ss << index++;
		
		auto parameter = ASTFactory::MakeNode<Operator::PARAMETER_DECLARATION>(
			Token{ ss.str(), TokenType::IDENTIFIER },
			ASTFactory::MakeLogicalNode(m_reserved.pool.at(t).token));
		auto event = VarTokenEvent::MakeParameter((*parameter)[0], (*parameter)[1]);
        Observable<VarTokenEvent>::notifyObservers(event);
			
		parameters.push_back(std::move(parameter));
		ss.clear();
	}

	auto bridgeFunction = ASTFactory::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(functionNameToken, std::move(parameters));

	auto functionEvent = VarTokenEvent::MakeFunction(*bridgeFunction);
	Observable<VarTokenEvent>::notifyObservers(functionEvent);

	auto functionBodyNode = ASTFactory::MakeEmptyNode();
	auto functionDeclarationNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionNameToken, std::move(bridgeFunction), std::move(functionBodyNode));

	auto statementEvent = FunctionTokenEvent{ *functionDeclarationNode, FunctionTokenEventType::DECLARATION_STATEMENT, functionNameToken.name() };
	Observable<FunctionTokenEvent>::notifyObservers(statementEvent);

	m_bridges.push_back(ASTFactory::MakeNode<Operator::BRIDGE>(std::move(functionDeclarationNode)));
}
