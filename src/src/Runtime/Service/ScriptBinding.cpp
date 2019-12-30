#include <fstream>
#include "Config/LoggerConfigLang.h"
#include "ScriptBinding.h"
#include "Service/ASTFactory.h"
#include "Service/SymbolTable.h"
#include "Service/TypeBuilder/TypeBuilder.h"
#include "Service/SymbolTableUpdater.h"
#include "Service/ScriptNameBuilder.h"

#include "Service/StatementParser.h"

ska::ScriptBindingAST::ScriptBindingAST(
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
	m_scriptAst(cache, m_name, std::vector<Token>{}),
	m_cacheAst(cache) {
	queryAST();
}

void ska::ScriptBindingAST::bindFunction(Type functionType, decltype(NativeFunction::function) f) {
	SLOG(LogLevel::Debug) << "Binding function \"" << functionType;
	auto field = BridgeField {std::move(functionType)};
	field.callback = std::move(f);
	assert(functionType.symbol() != nullptr);
	m_bindings.push_back(std::move(field));
}

const std::string& ska::ScriptBindingAST::templateName() const {
	return m_templateName;
}

void ska::ScriptBindingAST::queryAST() {
	auto found = m_cacheAst.atOrNull(m_templateName);
	if(found == nullptr) {
		SLOG(LogLevel::Info) << "Building template script " << m_templateName;
		auto file = std::ifstream { m_templateName };
		m_templateScript = m_parser.subParse(m_cacheAst, m_templateName, file);
		SLOG(LogLevel::Info) << "Template script built";
	} else {
		m_templateScript = std::make_unique<ScriptAST>(*found);
	}
}

ska::BridgeFunction ska::ScriptBindingAST::buildConstructorFromBindings(BridgeField constructorField) {
	auto constructor = BridgeFunction { constructorField };
	assert(!m_bindings.empty() && "Bridge is empty");
	for (auto& binding : m_bindings) {
		constructor.bindField(std::move(binding));
	}
	m_bindings = {};
	return constructor;
}

ska::ASTNodePtr ska::ScriptBindingAST::buildFunctionsAST(const BridgeFunction& constructor) {
	SLOG(LogLevel::Info) << "Building script " << m_scriptAst.name() << " from bridge";
	SLOG(LogLevel::Info) << "Current constructor is : " << constructor.name();

	return ASTFactory::MakeNode<Operator::BLOCK>(m_functionBuilder.makeFunction(m_scriptAst, constructor));
}
