#include "Config/LoggerConfigLang.h"
#include <memory>
#include "ScriptAST.h"
#include "Service/StatementParser.h"
#include "Service/ASTFactory.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::ScriptAST)

ska::ScriptAST::ScriptAST(ScriptCacheAST& scriptCache, const std::string& name, std::vector<Token> input, std::size_t startIndex) :
	m_cache(&scriptCache) {
	if(m_cache->find(name) == m_cache->end()) {
		auto handle = std::unique_ptr<ScriptHandleAST>(new ScriptHandleAST{ *m_cache, std::move(input), startIndex, name });
		SLOG(LogLevel::Info) << "Adding script AST " << name << " in cache";
		m_cache->emplace(name, std::move(handle));
	} else {
		SLOG(LogLevel::Info) << "Script AST " << name << " is already in cache";
		m_inCache = true;
	}
	m_handle = m_cache->at(name).get();
}

ska::ScriptASTPtr ska::ScriptAST::useImport(const std::string& name) {
	return existsInCache(name) ? std::make_unique<ScriptAST>(*m_cache, name, std::vector<Token>{}) : nullptr;
}

void ska::ScriptAST::astFromBridge(std::vector<ASTNodePtr> bindings) {
	assert(m_handle->m_ast == nullptr && "Script built from a bridge must be empty");

	auto functionListNodes = std::vector<ASTNodePtr>();
	if (!bindings.empty()) {
		functionListNodes.reserve(bindings.size());
	}

	for (auto& bridgeFunction : bindings) {
		auto functionName = bridgeFunction->name();
		auto functionVarDeclarationNode = ASTFactory::MakeNode<Operator::VARIABLE_DECLARATION>(std::move(Token{ functionName, TokenType::IDENTIFIER , {} }), std::move(bridgeFunction));
		functionListNodes.push_back(std::move(functionVarDeclarationNode));
	}

	m_handle->m_ast = ASTFactory::MakeNode<Operator::BLOCK>(std::move(functionListNodes));
	m_handle->m_bridged = true;
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


ska::ASTNode& ska::ScriptAST::fromBridge(std::vector<ASTNodePtr> bindings) {
	astFromBridge(std::move(bindings));
	return *m_handle->m_ast;
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

