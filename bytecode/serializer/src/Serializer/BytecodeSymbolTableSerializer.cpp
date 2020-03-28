#include "Serializer/Config/LoggerSerializer.h"
#include "BytecodeSymbolTableSerializer.h"
#include "BytecodeCommonSerializer.h"
#include "BytecodeOperandSerializer.h"
#include "NodeValue/Symbol.h"
#include "Generator/Value/BytecodeScriptCache.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::bytecode::SymbolTableSerializer);

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

void ska::bytecode::SymbolTableSerializer::writeFull(SerializerOutput output, std::size_t id) {
	getMapBuilder(id).write(output, *this);
	output.validate();
}

void ska::bytecode::SymbolTableSerializer::writeFull(SerializerOutput& output, const TreeSymbolTableMapBuilder::ReverseIndexSymbolMap& reversedMap) {
	for (const auto& [key, symbol] : reversedMap) {
		LOG_INFO << "Writing symbol \"" << symbol->name() << "\"";
		writeIfExists(output, symbol);
	}
}

void ska::bytecode::SymbolTableSerializer::writeIfExists(SerializerOutput& output, const Symbol* value) {
	if (value == nullptr) {
		throw std::runtime_error("cannot serialize a null symbol");
	}

	value->type().serialize(output, *this, false);
	writeSymbolRefAndParents(output.acquireMemory<2 * sizeof(Chunk)>("symbol ref & parent"), *value);
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

	auto result = std::to_string(scriptId) + "." + relativeScriptKey;
	LOG_DEBUG << "Getting symbol \"" << value.name() << "\" absolute script key \"" << result << "\"";

	return result;
}

void ska::bytecode::SymbolTableSerializer::writeTypeAndSymbolOneLevel(SerializerSafeZone<sizeof(uint32_t) + sizeof(uint8_t) + 2 * sizeof(Chunk)> output, const Symbol* symbol, const Type& type) {
	auto rawType = type.type();
	output.acquireMemory<sizeof(uint8_t)>("raw").write(static_cast<uint8_t>(rawType));

	LOG_INFO << "Type \"" << type << "\" is being serialized with " << type.size() << " compound types";

	if (symbol != nullptr) {
		writeSymbolRefAndParents(output.acquireMemory<2 * sizeof(Chunk)>("Symbol ref & parent"), *symbol);
	} else {
		CommonSerializer::writeNullChunk<2>(output.acquireMemory<2 * sizeof(Chunk)>("no symbol ref & parent"));
	}

	output.acquireMemory<sizeof(uint32_t)>("type children").write(static_cast<uint32_t>(type.size()));
}

void ska::bytecode::SymbolTableSerializer::write(SerializerOutput& output, const Symbol* symbol, const Type& type) {
	writeTypeAndSymbolOneLevel(output.acquireMemory<sizeof(uint8_t) + sizeof(uint32_t) + 2 * sizeof(Chunk)>("type"), symbol, type);

	for (auto& childType : type) {
		LOG_INFO << "\t\tChild type " << childType;
		LOG_INFO << "%13c\t\twith Raw type : " << ExpressionTypeSTR[static_cast<std::size_t>(childType.type())];
		LOG_INFO << "%13c\t\twith " << childType.size() << " children";

		// Recursive call into this function is done inside Type::serialize
		childType.serialize(output, *this, true);
	}
}

void ska::bytecode::SymbolTableSerializer::writeSymbolRefBody(SerializerSafeZone<sizeof(Chunk)> output, const Symbol& value) {
	auto [scriptId, operand] = extractGeneratedOperandFromSymbol(value);
	
	//LOG_INFO << "%13c\twith Raw operand " << operand;
	//LOG_INFO << "%13c\twith " << value.size() << " children";

	// Operand (???)
	// OperandSerializer::write(*m_cache, buffer, natives, operand);

	auto absoluteScriptKey = getAbsoluteScriptKey(scriptId, value);

	LOG_INFO << "%13cSymbol Name refered : " << value.name() << " with key : " << absoluteScriptKey;

	output.write(std::move(absoluteScriptKey));
}


void ska::bytecode::SymbolTableSerializer::writeSymbolRefAndParents(SerializerSafeZone<2 * sizeof(Chunk)> output, const Symbol& value) {
	writeSymbolRefBody(output.acquireMemory<sizeof(Chunk)>("Symbol ref"), value);

	if (&value == value.master()) {
		CommonSerializer::writeNullChunk<1>(output.acquireMemory<sizeof(Chunk)>("null"));
	} else {
		LOG_INFO << "%13c\twith master : " << value.master()->name();
		writeSymbolRefBody(output.acquireMemory<sizeof(Chunk)>("parent"), *value.master());
	}
}

