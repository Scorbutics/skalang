#include "Config/LoggerConfigLang.h"
#include "BytecodeSymbolTableDeserializerHelper.h"
#include "Generator/Value/BytecodeScriptCache.h"
#include "Base/Serialization/SerializerSafeZone.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::bytecode::SymbolTableDeserializerHelper);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::SymbolTableDeserializerHelper)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::SymbolTableDeserializerHelper)
#define LOG_ERROR SLOG_STATIC(ska::LogLevel::Error, ska::bytecode::SymbolTableDeserializerHelper)

ska::bytecode::SymbolTableDeserializerHelper::SymbolTableDeserializerHelper(ScriptCache& cache) :
	m_cache(&cache) {
}

ska::bytecode::TreeMapSymbolTableBuilder& ska::bytecode::SymbolTableDeserializerHelper::getSymbolTableBuilder(const std::string& scriptName) {
	if (m_symbolTableBuilder[scriptName] == nullptr) {
		// Virtually rebuild a ScriptAST to refer its symbol table
		if (m_cache->astCache.atOrNull(scriptName) == nullptr) {
			auto scriptAST = ScriptAST{ m_cache->astCache, scriptName, {} };
			LOG_INFO << "Script AST \"" << scriptName << "\" was empty-built for deserialization of its symbol table";
		}
		m_symbolTableBuilder[scriptName] = std::make_unique<TreeMapSymbolTableBuilder>(m_cache->astCache.at(scriptName).symbols());
	}

	return *m_symbolTableBuilder[scriptName];
}

ska::Symbol& ska::bytecode::SymbolTableDeserializerHelper::buildSymbol(detail::SerializerSafeZone& zone, const std::string& absoluteScriptKey, std::string symbolName) {
	Symbol* symbol = nullptr;

	if (!absoluteScriptKey.empty()) {
		const auto scriptDelimiter = absoluteScriptKey.find_first_of('.');
		std::size_t scriptNativeStrId;
		if (scriptDelimiter != std::string::npos) {
			scriptNativeStrId = std::atoi(absoluteScriptKey.substr(0, scriptDelimiter).c_str());
		} else {
			scriptNativeStrId = std::atoi(absoluteScriptKey.c_str());
		}
		const auto& scriptName = zone.ref(scriptNativeStrId);
		symbol = getSymbolTableBuilder(scriptName).value(absoluteScriptKey, symbolName);
	} else {
		LOG_ERROR << "Empty symbol script key provided";
	}
	
	if (symbol == nullptr) {
		auto ss = std::stringstream{};
		ss << "bad symbol key provided \"" << absoluteScriptKey << "\"";
		throw std::runtime_error(ss.str());
	}

	LOG_DEBUG << "Getting symbol \"" << symbol->name() << "\" from absolute script key \"" << absoluteScriptKey << "\"";

	return *symbol;
}
