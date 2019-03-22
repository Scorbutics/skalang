#include "ScriptBinding.h"
#include "Service/ASTFactory.h"
#include "Service/SymbolTable.h"
#include "Service/Script.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScriptBinding);

ska::ScriptBridge ska::ScriptBinding::buildScriptBridge(Script& script, const std::string& scriptName, std::vector<BridgeFunctionPtr> functions) {
	auto functionListNodes = functions.empty() ? std::vector<ASTNodePtr>() : std::vector<ASTNodePtr> (functions.size());
	for(auto& bridgeFunction : functions) {
		auto functionName = bridgeFunction->node->name();
		functionListNodes.push_back(std::move(bridgeFunction->node));
		script.memory().emplace(functionName, NodeValue{ std::move(bridgeFunction) });
	}

	auto bridgeNode = ASTFactory::MakeNode<Operator::USER_DEFINED_OBJECT>(std::move(functionListNodes));
	auto varNode = ASTFactory::MakeNode<Operator::VARIABLE_DECLARATION>(std::move(Token{ scriptName, TokenType::IDENTIFIER }), std::move(bridgeNode));
	
	auto event = VarTokenEvent::Make<VarTokenEventType::VARIABLE_DECLARATION>(*varNode, script.symbols());
	
	Observable<VarTokenEvent>::addObserver(script.symbols());
	Observable<VarTokenEvent>::notifyObservers(event);
	Observable<VarTokenEvent>::removeObserver(script.symbols());

	return ScriptBridge { scriptName, std::move(varNode) };
}
