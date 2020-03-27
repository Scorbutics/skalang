#include "BytecodeSymbolTableSerializer.h"
#include "BytecodeCommonSerializer.h"
#include "BytecodeOperandSerializer.h"
#include "NodeValue/Symbol.h"
#include "Generator/Value/BytecodeScriptCache.h"

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::bytecode::SymbolTableSerializer);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::SymbolTableSerializer)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::SymbolTableSerializer)

ska::bytecode::SymbolTableSerializer::SymbolTableSerializer(const ScriptCache& cache) :
	m_cache(&cache) {
}

ska::bytecode::TreeSymbolTableMapBuilder& ska::bytecode::SymbolTableSerializer::getMapBuilder(std::size_t id) {
	if (id >= m_mapBuilder.size()) {
		m_mapBuilder.resize(id + 1);
	}

	if (!m_mapBuilder[id].has_value()) {
		m_mapBuilder[id] = TreeSymbolTableMapBuilder { m_cache->at(id).program().symbols() };
	}
	
	return m_mapBuilder[id].value();
}

void ska::bytecode::SymbolTableSerializer::writeFull(std::size_t id, std::unordered_map<std::string, std::size_t>& natives, std::stringstream& buffer) {
	getMapBuilder(id).write(buffer, natives, *this);
}

void ska::bytecode::SymbolTableSerializer::writeFull(std::stringstream& buffer, std::unordered_map<std::string, std::size_t>& natives, const TreeSymbolTableMapBuilder::ReverseIndexSymbolMap& reversedMap) {
	for (const auto& [key, symbol] : reversedMap) {
		writeIfExists(buffer, natives, symbol);
	}
}

void ska::bytecode::SymbolTableSerializer::writeIfExists(std::stringstream& buffer, std::unordered_map<std::string, std::size_t>& natives, const Symbol* value) {
	if (value == nullptr) {
		throw std::runtime_error("cannot serialize a null symbol");
	}

	auto [scriptId, operand] = extractGeneratedOperandFromSymbol(*value);
	LOG_INFO << "%13cName : " << value->name();
	LOG_INFO << "%13c\twith Raw operand " << operand;
	LOG_INFO << "%13c\twith " << value->type().size() << " children";
	LOG_INFO << "%13c\twith Raw type : " << ExpressionTypeSTR[static_cast<std::size_t>(value->type().type())];
	
	OperandSerializer::write(*m_cache, buffer, natives, operand);

	if (m_mapBuilder.size() <= scriptId) {
		throw std::runtime_error("bad script id : \"" + std::to_string(scriptId) + "\" id is not already known by map builder");
	}

	auto absoluteScriptKey = getAbsoluteScriptKey(scriptId, *value);

	// TODO call write for symbol type ?

	/*
		TODO write symbol data

	if (m_symbols.find(value) == m_symbols.end()) {
		m_symbols.insert(value);
		*this << value->type();
	} else {
		LOG_DEBUG << "Symbol \"" << value->name() << "\" already registered, linked";
	}
	*/
}

std::pair<std::size_t, ska::bytecode::Operand> ska::bytecode::SymbolTableSerializer::extractGeneratedOperandFromSymbol(const Symbol& symbol) {
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

std::string ska::bytecode::SymbolTableSerializer::getAbsoluteScriptKey(std::size_t scriptId, const Symbol& value) {
	auto relativeScriptKey = getMapBuilder(scriptId).key(value);
	if (relativeScriptKey.empty()) {
		auto ss = std::stringstream{};
		ss << "bad symbol key retrieved in map for symbol \"" << value.type() << "\"";
		throw std::runtime_error(ss.str());
	}

	return std::to_string(scriptId) + "." + relativeScriptKey;
}

void ska::bytecode::SymbolTableSerializer::write(std::stringstream& buffer, std::unordered_map<std::string, std::size_t>& natives, const Type value) {
	auto rawType = value.type();
	buffer.write(reinterpret_cast<const char*>(&rawType), sizeof(uint8_t));

	LOG_INFO << "Type \"" << value << "\" is being serialized with " << value.size() << " compound types";

	auto index = std::size_t{ 0 };
	for (auto& childType : value) {
		auto operand = Operand{};
		LOG_INFO << "\t\tChild type " << childType;
		/*if (childType.hasSymbol()) {
			operand = extractGeneratedOperandFromSymbol(*childType.symbol());
		}
		LOG_INFO << "%13cChild " << index << "\t\twith name "<< (childType.symbol() ? childType.symbol()->name() : "");*/
		LOG_INFO << "%13c\t\twith Raw operand " << operand;
		LOG_INFO << "%13c\t\twith " << childType.size() << " children";
		LOG_INFO << "%13c\t\twith Raw type : " << ExpressionTypeSTR[static_cast<std::size_t>(childType.type())];
		
		CommonSerializer::write(buffer, static_cast<std::size_t>(childType.type()));
		CommonSerializer::write(buffer, childType.size());
		OperandSerializer::write(*m_cache, buffer, natives, operand);
		
		// TODO rethink about writing a type

		// TODO reccursive ?
		//*this << childType;

		index++;
	}
}

