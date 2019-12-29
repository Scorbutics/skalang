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
	m_functionBuilder(typeBuilder, symbolTypeUpdater, reserved),
	m_name(ScriptNameDeduce("", "bind:" + scriptName)),
	m_templateName(ScriptNameDeduce("", templateName)),
    m_script(cache, m_name, std::vector<Token>{}),
	m_cache(cache) {
	queryAST();
}

void ska::ScriptBindingBase::bindFunction(Type functionType, decltype(NativeFunction::function) f) {
	SLOG(LogLevel::Debug) << "Binding function \"" << functionType;
	auto field = BridgeField {std::move(functionType)};
	field.callback = std::move(f);
	assert(functionType.symbol() != nullptr);
	m_bindings.push_back(std::move(field));
}

const std::string& ska::ScriptBindingBase::templateName() const {
	return m_templateName;
}

void ska::ScriptBindingBase::queryAST() {
	auto found = m_cache.atOrNull(m_templateName);
	if(found == nullptr) {
		SLOG(LogLevel::Info) << "Building template script " << m_templateName;
		auto file = std::ifstream { m_templateName };
		m_templateScript = m_parser.subParse(m_cache, m_templateName, file);
		SLOG(LogLevel::Info) << "Template script built";
	} else {
		m_templateScript = std::make_unique<ScriptAST>(*found);
	}
}

ska::ASTNode& ska::ScriptBindingBase::buildFunctionsAST(ScriptAST& target, BridgeField constructorField) {
	SLOG(LogLevel::Info) << "Building script " << target.name() << " ( " << m_script.name() << ") from bridge";
	SLOG(LogLevel::Info) << "Current constructor is : " << constructorField.name();

	assert(!m_bindings.empty() && "Bridge is empty");
	assert(target.id() == m_script.id());

	auto constructor = BridgeFunction{ std::move(constructorField) };
	for(auto& binding : m_bindings) {
		constructor.bindField(std::move(binding));
	}
	m_bindings = {};

	auto astRoot = ASTFactory::MakeNode<Operator::BLOCK>(m_functionBuilder.makeFunction(m_script, std::move(constructor)));
	return m_script.fromBridge(std::move(astRoot));
}
