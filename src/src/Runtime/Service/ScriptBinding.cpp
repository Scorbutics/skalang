#include <fstream>
#include "Config/LoggerConfigLang.h"
#include "ScriptBinding.h"

#include "Service/ASTFactory.h"
#include "Service/ScriptNameBuilder.h"
#include "Service/StatementParser.h"

ska::ScriptBindingAST::ScriptBindingAST(
	ModuleConfiguration& config,
	std::string scriptName,
	std::string templateName) :
	m_parser(config.parser),
	m_functionBuilder(config.typeBuilder, config.reservedKeywords),
	m_name(ScriptNameDeduce("", "bind:" + scriptName)),
	m_templateName(ScriptNameDeduce("", templateName)),
	m_scriptAst(config.scriptAstCache, m_name, std::vector<Token>{}),
	m_cacheAst(config.scriptAstCache) {
	queryAST();
}

void ska::ScriptBindingAST::bindFunction(const Symbol& function, decltype(NativeFunction::function) f) {
	SLOG(LogLevel::Debug) << "Binding function \"" << function;
	auto field = BridgeField { function };
	field.callback = std::move(f);
	m_bindings.push_back(std::move(field));
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

void ska::ScriptBindingAST::fillConstructorWithBindings(BridgeFunction& constructor) {
	assert(!m_bindings.empty() && "Bridge is empty");
	for (auto& binding : m_bindings) {
		constructor.bindField(std::move(binding));
	}
	m_bindings = {};
}

ska::ASTNodePtr ska::ScriptBindingAST::buildFunctionsAST(BridgeFunction& constructor) {
	SLOG(LogLevel::Info) << "Building script " << m_scriptAst.name() << " from bridge";
	SLOG(LogLevel::Info) << "Current constructor is : " << constructor.name();
	fillConstructorWithBindings(constructor);

	if (constructor.isVoid()) {
		return ASTFactory::MakeNode<Operator::BLOCK>(m_functionBuilder.makeFieldList(m_scriptAst, constructor));
	} 

	return ASTFactory::MakeNode<Operator::BLOCK>(m_functionBuilder.makeFunction(m_scriptAst, constructor));
}
