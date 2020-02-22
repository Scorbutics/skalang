#include "Config/LoggerConfigLang.h"
#include <memory>
#include "ScriptAST.h"
#include "Service/StatementParser.h"
#include "Service/ASTFactory.h"
#include "Runtime/Value/NativeFunction.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScriptAST)

ska::ScriptAST::ScriptAST(ScriptCacheAST& scriptCache, const std::string& name, std::vector<Token> input, std::size_t startIndex, std::size_t scriptId) :
	m_cache(&scriptCache) {
	if(m_cache->find(name) == m_cache->end()) {
		auto handle = std::unique_ptr<ScriptHandleAST>(new ScriptHandleAST{ *m_cache, std::move(input), startIndex, name });
		SLOG(LogLevel::Info) << "Adding script AST " << name << " in cache";
		m_cache->emplace(name, std::move(handle), true);		
	} else {
		SLOG(LogLevel::Info) << "Script AST " << name << " is already in cache";
		m_inCache = true;
	}
	m_handle = &m_cache->at(name);
}

ska::ScriptASTPtr ska::ScriptAST::useImport(const std::string& name) {
	return existsInCache(name) ? std::make_unique<ScriptAST>(*m_cache, name, std::vector<Token>{}) : nullptr;
}

ska::ASTNode& ska::ScriptAST::fromBridge(ASTNodePtr astRoot) {
	assert(m_handle->m_ast == nullptr && "Script built from a bridge must be empty");


	m_handle->m_ast = std::move(astRoot);
	m_handle->m_bridged = true;
	return *m_handle->m_ast;
}

ska::ScriptASTPtr ska::ScriptAST::subParse(StatementParser& parser, const std::string& name, std::ifstream& file) {
	return parser.subParse(m_handle->m_cache, name, file);
}

void ska::ScriptAST::parse(StatementParser& parser, bool listen) {
	if(m_inCache) {
		//No more parsing if script is already in-cache.
		return;
	}

	if(!listen) {
		m_handle->m_ast = parser.parse(*this);
		return;
	}

	auto lock = m_handle->m_symbols.listenParser(parser);
	auto result = parser.parse(*this);
	lock.release();
	m_handle->m_ast = std::move(result);
	return;
}

ska::ASTNodePtr ska::ScriptAST::statement(StatementParser& parser) {
	return parser.statement(*this);
}

ska::ASTNodePtr ska::ScriptAST::optstatement(StatementParser& parser, const Token& mustNotBe) {
	return parser.optstatement(*this, mustNotBe);
}

ska::ASTNodePtr ska::ScriptAST::expr(StatementParser& parser) {
	return parser.expr(*this);
}

ska::ASTNodePtr ska::ScriptAST::optexpr(StatementParser& parser, const Token& mustNotBe) {
	return parser.optexpr(*this, mustNotBe);
}

