#include "Config/LoggerConfigLang.h"
#include "ScriptBinding.h"
#include "Service/ASTFactory.h"
#include "Service/SymbolTable.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/SymbolTableUpdater.h"
#include "Service/ScriptNameBuilder.h"

ska::ScriptBinding::ScriptBinding(
  ScriptCacheAST& cache,
	std::string scriptName,
	TypeBuilder& typeBuilder,
	SymbolTableUpdater& symbolTypeUpdater,
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

ska::ScriptBinding::~ScriptBinding() {
	observable_priority_queue<VarTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<VarTokenEvent>::removeObserver(m_symbolTypeUpdater);
	observable_priority_queue<VarTokenEvent>::removeObserver(m_script.symbols());
}

void ska::ScriptBinding::registerAST(ASTNode& scriptAst) {
  for (auto& functionVarDeclaration : scriptAst) {
		auto event = VarTokenEvent::Make<VarTokenEventType::VARIABLE_DECLARATION>(*functionVarDeclaration, m_script);
		observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	}
}

ska::ASTNode& ska::ScriptBinding::import(StatementParser& parser, std::vector<std::pair<std::string, std::string>> imports) {
	auto importBlock = m_functionBinder.import(parser, m_script, std::move(imports));
	m_imports.push_back(std::move(importBlock));
  return *m_imports.back().get();
}
