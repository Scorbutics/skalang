#include "ScriptBinding.h"
#include "Service/ASTFactory.h"
#include "Service/SymbolTable.h"
#include "Service/Script.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/SymbolTableTypeUpdater.h"

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
	m_name(std::move(scriptName)),
	m_script(cache, m_name + ".miniska", std::vector<Token>{}) {
	observable_priority_queue<VarTokenEvent>::addObserver(m_typeBuilder);
	observable_priority_queue<VarTokenEvent>::addObserver(m_symbolTypeUpdater);
	observable_priority_queue<VarTokenEvent>::addObserver(m_script.symbols());
}

ska::ScriptBridge::~ScriptBridge() {
	observable_priority_queue<VarTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<VarTokenEvent>::removeObserver(m_symbolTypeUpdater);
	observable_priority_queue<VarTokenEvent>::removeObserver(m_script.symbols());
}

void ska::ScriptBridge::build() {
	assert(!m_bindings.empty() && "Bridge is empty");
	auto& scriptAst = m_script.fromBridge( std::move(m_bindings));
	
	for (auto& functionVarDeclaration : scriptAst) {
		auto event = VarTokenEvent::Make<VarTokenEventType::VARIABLE_DECLARATION>(*functionVarDeclaration, m_script);
		observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	}

	m_bindings = { };
}
