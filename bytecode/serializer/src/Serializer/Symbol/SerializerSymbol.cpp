#include "Config/LoggerConfigLang.h"
#include "SerializerSymbol.h"
#include "BytecodeSymbolTableDeserializer.h"
#include "BytecodeSymbolTableSerializer.h"

namespace ska {
	struct SerializerTypeTraitsSymbolLog;
}

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::SerializerTypeTraitsSymbolLog);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::SerializerTypeTraitsSymbolLog)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::SerializerTypeTraitsSymbolLog)

void ska::SerializerTypeTraits<ska::Symbol*>::Read(SerializerSafeZone<BytesRequired>& zone, Symbol*& symbol, bytecode::SymbolTableDeserializerHelper& helper) {
	auto absoluteScriptKey = zone.read<std::string>();
	auto symbolName = zone.read<std::string>();
	symbol = &helper.buildSymbol(zone, absoluteScriptKey, symbolName);

	LOG_INFO << "%13cSymbol Name refered : " << symbolName << " with key : " << absoluteScriptKey;
	const auto hasMaster = zone.read<uint8_t>() != 0;
	if (!hasMaster) {
		LOG_INFO << "%13cNo parent symbol for " << symbolName;
		zone.acquireMemory<2 * sizeof(bytecode::Chunk)>("No parent").readNull<2 * sizeof(bytecode::Chunk)>();
	} else {
		auto absoluteScriptParentKey = zone.read<std::string>();
		auto parentSymbolName = zone.read<std::string>();
		LOG_INFO << "%13c\twith master : " << parentSymbolName;
		auto& master = helper.buildSymbol(zone, absoluteScriptKey, parentSymbolName);
		master.implement(*symbol);
	}
}

void ska::SerializerTypeTraits<ska::Symbol*>::Write(SerializerSafeZone<BytesRequired>& zone, const Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper) {
	WriteSymbolRefBody(zone.acquireMemory<2 * sizeof(bytecode::Chunk)>("Symbol ref"), symbol, helper);

	const auto hasMaster = &symbol != symbol.master();
	zone.write(static_cast<uint8_t>(hasMaster));
	if (!hasMaster) {
		zone.writeNull<2*sizeof(bytecode::Chunk)>();
	} else {
		LOG_INFO << "%13c\twith master : " << symbol.master()->name();
		WriteSymbolRefBody(zone.acquireMemory<2 * sizeof(bytecode::Chunk)>("parent"), *symbol.master(), helper);
	}
}
	
void ska::SerializerTypeTraits<ska::Symbol*>::WriteSymbolRefBody(SerializerSafeZone<2 * sizeof(bytecode::Chunk)> zone, const Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper) {
	auto [scriptId, operand] = helper.extractGeneratedOperandFromSymbol(symbol);

	//LOG_INFO << "%13c\twith Raw operand " << operand;
	//LOG_INFO << "%13c\twith " << symbol.size() << " children";

	// Operand (???)
	// OperandSerializer::write(*m_cache, buffer, natives, operand);

	auto absoluteScriptKey = std::to_string(zone.ref(helper.getScriptName(scriptId))) + "." + helper.getRelativeScriptKey(scriptId, symbol);

	LOG_INFO << "%13cSymbol Name refered : " << symbol.name() << " with key : " << absoluteScriptKey;

	zone.write(std::move(absoluteScriptKey));

	zone.write(symbol.name());
}

