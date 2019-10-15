#include "Config/LoggerConfigLang.h"
#include "ScriptBinding.h"
#include "Service/ASTFactory.h"
#include "Service/SymbolTable.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/SymbolTableUpdater.h"
#include "Service/ScriptNameBuilder.h"

ska::ScriptBindingBase::ScriptBindingBase(
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

ska::ScriptBindingBase::~ScriptBindingBase() {
	observable_priority_queue<VarTokenEvent>::removeObserver(m_typeBuilder);
	observable_priority_queue<VarTokenEvent>::removeObserver(m_symbolTypeUpdater);
	observable_priority_queue<VarTokenEvent>::removeObserver(m_script.symbols());
}

void ska::ScriptBindingBase::registerAST(ASTNode& scriptAst) {
  for (auto& functionVarDeclaration : scriptAst) {
		auto event = VarTokenEvent::Make<VarTokenEventType::VARIABLE_DECLARATION>(*functionVarDeclaration, m_script);
		observable_priority_queue<VarTokenEvent>::notifyObservers(event);
	}
}

ska::BridgeImport ska::ScriptBindingBase::import(StatementParser& parser, std::pair<std::string, std::string> import) {
	auto importBridge = m_functionBinder.import(parser, m_script, std::move(import));
	m_imports.push_back(ASTFactory::MakeNode<Operator::BLOCK>(std::move(importBridge.node)));
  return { m_imports.back().get(), importBridge.script };
}
