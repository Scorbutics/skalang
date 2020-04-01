#include "Config/LoggerConfigLang.h"
#include "BytecodeSymbolTableSerializerHelper.h"
#include "Generator/Value/BytecodeScriptCache.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::bytecode::SymbolTableSerializerHelper);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::SymbolTableSerializerHelper)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::SymbolTableSerializerHelper)
#define LOG_ERROR SLOG_STATIC(ska::LogLevel::Error, ska::bytecode::SymbolTableSerializerHelper)

ska::bytecode::SymbolTableSerializerHelper::SymbolTableSerializerHelper(const ScriptCache& cache) :
	m_cache(&cache) {
}

ska::bytecode::TreeSymbolTableMapBuilder& ska::bytecode::SymbolTableSerializerHelper::getMapBuilder(std::size_t id) {
	if (id >= m_mapBuilder.size()) {
		m_mapBuilder.resize(id + 1);
	}

	if (m_mapBuilder[id] == nullptr) {
		m_mapBuilder[id] = std::make_unique<TreeSymbolTableMapBuilder>(m_cache->at(id).program().symbols());
	}

	return *m_mapBuilder[id];
}

std::pair<std::size_t, ska::bytecode::Operand> ska::bytecode::SymbolTableSerializerHelper::extractGeneratedOperandFromSymbol(const Symbol& symbol) {
	auto* info = m_cache->getSymbolInfo(symbol);
	if (info == nullptr) {
		throw std::runtime_error("unknown ast symbol \"" + symbol.name() + "\" detected during script bytecode serialization");
	}
	auto operand = m_cache->at(info->script).getSymbol(symbol);
	if (!operand.has_value()) {
		throw std::runtime_error("not generated symbol \"" + symbol.name() + "\" in ast detected during script bytecode serialization");
	}

	LOG_DEBUG << "Symbol \"" << symbol.name() << "\" as variable \"" << operand.value() << "\"";

	return std::make_pair(info->script, operand.value());
}

const std::string& ska::bytecode::SymbolTableSerializerHelper::getScriptName(const std::size_t scriptId) const {
	return m_cache->at(scriptId).name();
}

std::string ska::bytecode::SymbolTableSerializerHelper::getRelativeScriptKey(std::size_t scriptId, const Symbol& value) {
	auto relativeScriptKey = getMapBuilder(scriptId).key(value);
	if (relativeScriptKey.empty()) {
		auto ss = std::stringstream{};
		ss << "bad symbol key retrieved in map for symbol \"" << value.type() << "\"";
		throw std::runtime_error(ss.str());
	}

	auto result = relativeScriptKey;
	LOG_DEBUG << "Getting symbol \"" << value.name() << "\" relative script key \"" << result << "\"";

	return result;
}

