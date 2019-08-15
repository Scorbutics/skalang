#include "Config/LoggerConfigLang.h"
#include <memory>
#include "Script.h"
#include "Service/StatementParser.h"
#include "Service/ASTFactory.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::Script)

ska::Script::Script(ScriptCache& scriptCache, const std::string& name, std::vector<Token> input, std::size_t startIndex) :
	m_cache(scriptCache), 
	m_ast{ ScriptAST{ m_cache.astCache, name, std::move(input), startIndex } }{
	m_handle = buildHandle(m_cache, *m_ast.handle(), name, m_inCache);
}

ska::Script::Script(ScriptCache& scriptCache, ScriptAST& parentScriptAST, const std::string& fullName) :
	m_cache(scriptCache),
	m_ast(*m_cache.astCache.at(fullName)) {
	m_handle = buildHandle(m_cache, *m_ast.handle(), fullName, m_inCache);
}

ska::ScriptHandle* ska::Script::buildHandle(ScriptCache& cache, ScriptHandleAST& handleAST, const std::string& name, bool& inCache) {
	if (cache.cache.find(name) == cache.cache.end()) {
		auto handle = std::unique_ptr<ScriptHandle>(new ScriptHandle{ cache, handleAST });
		SLOG_STATIC(LogLevel::Info, ska::Script) << "Adding script " << name << " in cache";
		cache.cache.emplace(name, std::move(handle));
	} else {
		SLOG_STATIC(LogLevel::Info, ska::Script) << "Script " << name << " is already in cache";
		inCache = true;
	}
	return cache.cache.at(name).get();
}

void ska::Script::memoryFromBridge(const ASTNode& declaredAstBlock, std::vector<BridgeMemory> bindings) {
	assert(declaredAstBlock.size() == bindings.size() && "Cannot create memory from this ast");

	//steal already existing first child content into the current scope
	m_handle->m_currentMemory->stealFirstChildContent();
	
	auto& astRoot = declaredAstBlock;
	auto index = std::size_t{ 0 };
	auto lock = pushNestedMemory(false);
	for (auto& bridgeFunction : bindings) {
		auto functionName = astRoot[index].name();
		m_handle->m_currentMemory->emplace(functionName, NodeValue{ std::move(bridgeFunction) });
		index++;
	}
	lock.release();
}

ska::ASTNode& ska::Script::fromBridge(std::vector<BridgeMemory> bindings) {
	
	auto bindingsAST = std::vector<ASTNodePtr>{};
	if (!bindings.empty()) {
		bindingsAST.reserve(bindings.size());
		std::transform(bindings.begin(), bindings.end(), std::back_inserter(bindingsAST), [](auto& el) {
			return std::move(el->node);
		});
	}
	m_ast.fromBridge(std::move(bindingsAST));
	memoryFromBridge(m_ast.rootNode(), std::move(bindings));
	return m_ast.rootNode();
}

ska::ScriptPtr ska::Script::useImport(const std::string& name) {
	return existsInCache(name) ? std::make_unique<Script>(m_cache, name, std::vector<Token>{}) : nullptr;
}

ska::ScriptPtr ska::Script::createImport(const std::string& name) {
	return std::make_unique<Script>( m_cache, m_ast, name );
}
