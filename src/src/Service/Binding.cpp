#include <functional>
#include "Service/ReservedKeywordsPool.h"
#include "Interpreter/Interpreter.h"
#include "Config/LoggerConfigLang.h"
#include "Service/SymbolTable.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/SymbolTableTypeUpdater.h"
#include "ASTFactory.h"
#include "Binding.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Binding)

ska::Binding::Binding(Interpreter& interpreter, SymbolTable& symbolTable, TypeBuilder& typeBuilder, SymbolTableTypeUpdater& symbolTypeUpdater, const ReservedKeywordsPool& reserved) :
	m_observer(symbolTable),
	m_interpreter(interpreter),
    m_reserved(reserved),
	m_typeBuilder(typeBuilder),
	m_symbolTypeUpdater(symbolTypeUpdater) {
	Observable<VarTokenEvent>::addObserver(m_observer);
	Observable<FunctionTokenEvent>::addObserver(m_observer);
	Observable<BlockTokenEvent>::addObserver(m_observer);
	Observable<BridgeTokenEvent>::addObserver(m_observer);
	Observable<VarTokenEvent>::addObserver(m_typeBuilder);
	Observable<FunctionTokenEvent>::addObserver(m_typeBuilder);
	Observable<BridgeTokenEvent>::addObserver(m_typeBuilder);
	Observable<VarTokenEvent>::addObserver(m_symbolTypeUpdater);
}

ska::Binding::~Binding() {
	Observable<VarTokenEvent>::removeObserver(m_symbolTypeUpdater);
	Observable<BridgeTokenEvent>::removeObserver(m_typeBuilder);
	Observable<FunctionTokenEvent>::removeObserver(m_typeBuilder);
	Observable<VarTokenEvent>::removeObserver(m_typeBuilder);
	Observable<BridgeTokenEvent>::removeObserver(m_observer);
	Observable<BlockTokenEvent>::removeObserver(m_observer);
	Observable<FunctionTokenEvent>::removeObserver(m_observer);
	Observable<VarTokenEvent>::removeObserver(m_observer);
}

/*
ska::ASTNode& ska::Binding::bindToScript(const std::string& scriptName) {
	auto importNameNode = ASTFactory::MakeLogicalNode(Token{ scriptName, TokenType::IDENTIFIER });
	auto startEvent = BlockTokenEvent{ *importNameNode, BlockTokenEventType::START };
	Observable<BlockTokenEvent>::notifyObservers(startEvent);
}
*/

void ska::Binding::bindSymbol(const std::string& functionName, std::vector<std::string> typeNames) {
	
	//Build the function
	auto functionNameToken = Token{ functionName, TokenType::IDENTIFIER };

	auto nodeBlock = ASTFactory::MakeNode<Operator::BLOCK>();
	auto startEvent = BlockTokenEvent{ *nodeBlock, BlockTokenEventType::START };
	Observable<BlockTokenEvent>::notifyObservers(startEvent);

	auto functionNameNode = ASTFactory::MakeLogicalNode(functionNameToken);
	auto declarationEvent = FunctionTokenEvent{ *functionNameNode, FunctionTokenEventType::DECLARATION_NAME, functionNameToken.name() };
	Observable<FunctionTokenEvent>::notifyObservers(declarationEvent);

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
			auto event = VarTokenEvent::MakeParameter(*parameter, (*parameter)[0]);
			Observable<VarTokenEvent>::notifyObservers(event);
			parameters.push_back(std::move(parameter));
		}
		ss.clear();
	}

	auto bridgeFunction = ASTFactory::MakeNode<Operator::FUNCTION_PROTOTYPE_DECLARATION>(functionNameToken, std::move(parameters));

	auto functionEvent = VarTokenEvent::MakeFunction(*bridgeFunction);
	Observable<VarTokenEvent>::notifyObservers(functionEvent);

	auto functionDeclarationNode = ASTFactory::MakeNode<Operator::FUNCTION_DECLARATION>(functionNameToken, std::move(bridgeFunction), ASTFactory::MakeNode<Operator::BLOCK>());

	auto statementEvent = FunctionTokenEvent{ *functionDeclarationNode, FunctionTokenEventType::DECLARATION_STATEMENT, functionNameToken.name() };
	Observable<FunctionTokenEvent>::notifyObservers(statementEvent);
	
	auto endEvent = BlockTokenEvent{ *functionDeclarationNode, BlockTokenEventType::END };
	Observable<BlockTokenEvent>::notifyObservers(endEvent);

	auto importNameNode = ASTFactory::MakeLogicalNode(Token{ "binding.miniska", TokenType::IDENTIFIER });

	//nodeBlock = ASTFactory::MakeNode<Operator::BLOCK>(std::move(functionDeclarationNode));

	//Fill the script with the function
	
	/*auto exportFields = std::vector<ASTNodePtr>{};
	auto allFields = std::vector<ASTNodePtr>{};
	exportFields.push_back(ASTFactory::MakeLogicalNode(ska::Token{ (*functionDeclarationNode)[0].name(), (*functionDeclarationNode)[0].tokenType() }));
	allFields.push_back(std::move(functionDeclarationNode));

	auto importNode = ASTFactory::MakeNode<Operator::IMPORT>(
		ASTFactory::MakeEmptyNode(),
		std::move(importNameNode),
		ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(exportFields)),
		ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(),
		ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(allFields)));
	*/

	auto bridgeNode = ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(functionDeclarationNode));

	/*auto event = BridgeTokenEvent{ *bridgeNode };
	Observable<BridgeTokenEvent>::notifyObservers(event);*/



	auto varNode = ASTFactory::MakeNode<Operator::VARIABLE_DECLARATION>(std::move(Token{ "binding.miniska", TokenType::IDENTIFIER }), std::move(bridgeNode));
	
	auto event = VarTokenEvent::Make<VarTokenEventType::VARIABLE_DECLARATION>(*varNode);
	Observable<VarTokenEvent>::notifyObservers(event);
	
	m_bridges.push_back(std::move(varNode));
}
