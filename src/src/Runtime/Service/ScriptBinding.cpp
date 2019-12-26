#include <fstream>
#include "Config/LoggerConfigLang.h"
#include "ScriptBinding.h"
#include "Service/ASTFactory.h"
#include "Service/SymbolTable.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/SymbolTableUpdater.h"
#include "Service/ScriptNameBuilder.h"

#include "Service/StatementParser.h"

ska::ScriptBindingBase::ScriptBindingBase(
  StatementParser& parser,
	ScriptCacheAST& cache,
	std::string scriptName,
	std::string templateName,
	TypeBuilder& typeBuilder,
	SymbolTableUpdater& symbolTypeUpdater,
	const ReservedKeywordsPool& reserved) :
	m_parser(parser),
	m_typeBuilder(typeBuilder),
	m_symbolTypeUpdater(symbolTypeUpdater),
	m_reservedKeywordsPool(reserved),
	m_functionBinder(typeBuilder, symbolTypeUpdater, reserved),
	m_name(ScriptNameDeduce("", "bind:" + scriptName)),
	m_templateName(ScriptNameDeduce("", templateName)),
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

ska::BridgeImport ska::ScriptBindingBase::import(std::string constructorMethod, StatementParser& parser, std::pair<std::string, std::string> import) {
	auto importVariableName = import.first;
	auto importBridge = m_functionBinder.import(parser, m_script, std::move(import));
	auto blockNode = ASTFactory::MakeNode<Operator::BLOCK>(std::move(importBridge.node));
	auto refBlockNode = blockNode.get();
	SLOG(LogLevel::Debug) << "Binding import \"" << importVariableName << "\"";
	m_imports.emplace(importVariableName, std::move(blockNode));
  return { refBlockNode, importBridge.script, std::move(constructorMethod) };
}

const std::string& ska::ScriptBindingBase::templateName() const {
	return m_templateName;
}

void ska::ScriptBindingBase::generateAst() {
	auto found = m_cache.atOrNull(m_templateName);
	if(found == nullptr) {
		//auto templateScriptContent = std::ifstream { m_templateName };
		//if (templateScriptContent.fail()) {
			throw std::runtime_error("unable to open template script file with full-path \"" + m_templateName + "\"");
		//}
		//m_templateScript = m_parser.subParse(m_cache, m_templateName, templateScriptContent);
	} else {
		m_templateScript = std::make_unique<ScriptAST>(*found);
	}
}
