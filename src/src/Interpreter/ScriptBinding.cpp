#include "Config/LoggerConfigLang.h"
#include "ScriptBinding.h"
#include "Service/ASTFactory.h"
#include "Service/SymbolTable.h"
#include "Interpreter/Value/Script.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/SymbolTableTypeUpdater.h"
#include "Service/ScriptNameBuilder.h"

#include "Interpreter/InterpreterOperatorFunctionCall.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScriptBridge);

ska::ScriptBridge::ScriptBridge(
	ScriptCache& cache,
	std::string scriptName,
	TypeBuilder& typeBuilder,
	SymbolTableTypeUpdater& symbolTypeUpdater,
	const ReservedKeywordsPool& reserved) :
	m_typeBuilder(typeBuilder),
	m_symbolTypeUpdater(symbolTypeUpdater),
	m_functionBinder(typeBuilder, symbolTypeUpdater, reserved),
	m_name(ScriptNameDeduce("", "bind:" + scriptName)),
	m_script(cache, m_name, std::vector<Token>{}),
	m_cache(cache) {
	observable_priority_queue<VarTokenEvent>::addObserver(m_typeBuilder);
	observable_priority_queue<VarTokenEvent>::addObserver(m_symbolTypeUpdater);
	observable_priority_queue<VarTokenEvent>::addObserver(m_script.symbols());
}

ska::ScriptBridge::~ScriptBridge() {
	observable_priority_queue<VarTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<VarTokenEvent>::removeObserver(m_symbolTypeUpdater);
	observable_priority_queue<VarTokenEvent>::removeObserver(m_script.symbols());
}

void ska::ScriptBridge::buildFunctions() {
	assert(!m_bindings.empty() && "Bridge is empty");
	auto& scriptAst = m_script.fromBridge( std::move(m_bindings));
	
	for (auto& functionVarDeclaration : scriptAst) {
		auto event = VarTokenEvent::Make<VarTokenEventType::VARIABLE_DECLARATION>(*functionVarDeclaration, m_script);
		observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	}

	m_bindings = { };
}

ska::NodeValue ska::ScriptBridge::callFunction(Interpreter& interpreter, std::string importName, std::string functionName, std::vector<ska::NodeValue> parametersValues) {
	auto import = m_script.findInMemoryTree(importName);
	auto importedScript = import.first->nodeval<ska::ExecutionContext>();
	auto functionToCallMemory = importedScript.program().downMemory()(functionName);
	auto& functionToExecute = functionToCallMemory.first->nodeval<ska::ExecutionContext>();

	auto operateOnFunction = ska::Operation<ska::Operator::FUNCTION_DECLARATION>(functionToExecute);
	return ska::InterpreterOperationFunctionCallScriptWithParams(m_script, interpreter, functionToCallMemory.second, operateOnFunction, std::move(parametersValues)).asRvalue().object;
}

ska::MemoryLValue ska::ScriptBridge::accessMemory(std::string importName, std::string field) {
	auto& em = m_script.findInMemoryTree(importName).first->nodeval<ska::ExecutionContext>();
	auto& emScriptMemory = em.program().downMemory();
	return (emScriptMemory)(field);
}
