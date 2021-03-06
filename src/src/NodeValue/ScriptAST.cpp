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
		auto& scriptWanted = m_cache->at(name);
		if (!input.empty() && scriptWanted.m_input.emptyTokens()) {
			if (scriptWanted.m_ast != nullptr) {
				throw std::runtime_error("invalid ast script instantiation : tokens were provided but ast is already built");
			}
			scriptWanted.m_input = TokenReader{ std::move(input) };
			
			//m_inCache = true;
		} else {
			SLOG(LogLevel::Info) << "Script AST " << name << " is already in cache";
			m_inCache = true;
		}
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

void ska::ScriptAST::pushContext(ParsingContext context) {
	m_handle->m_parsingContexts.push_back(std::move(context));
}

void ska::ScriptAST::popContext() {
	if (!m_handle->m_parsingContexts.empty()) {
		m_handle->m_parsingContexts.pop_back();
	}
}

ska::Token* ska::ScriptAST::contextOf(ParsingContextType type, std::size_t maxCount) const {
	std::size_t countIndex = 0;
	for (auto it = m_handle->m_parsingContexts.rbegin(); it != m_handle->m_parsingContexts.rend() && (maxCount == 0 || countIndex < maxCount); it++) {
		if (it->type == type) {
			countIndex++;
			if (countIndex >= maxCount) {
				return &it->owner;
			}
		}
	}
	return nullptr;
}
