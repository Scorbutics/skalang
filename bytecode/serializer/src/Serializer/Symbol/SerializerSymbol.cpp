#include "Config/LoggerConfigLang.h"
#include "SerializerSymbol.h"
#include "BytecodeSymbolTableSerializer.h"

namespace ska {
	struct SerializerTypeTraitsSymbolLog;
}

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::SerializerTypeTraitsSymbolLog);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::SerializerTypeTraitsSymbolLog)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::SerializerTypeTraitsSymbolLog)

void ska::SerializerTypeTraits<ska::Symbol>::Read(SerializerSafeZone<BytesRequired>& zone, Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper) {
	//TODO
}

void ska::SerializerTypeTraits<ska::Symbol>::Write(SerializerSafeZone<BytesRequired>& zone, const Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper) {
	WriteSymbolRefBody(zone.acquireMemory<sizeof(bytecode::Chunk)>("Symbol ref"), symbol, helper);

	if (&symbol == symbol.master()) {
		zone.acquireMemory<sizeof(bytecode::Chunk)>("null").writeNull<sizeof(bytecode::Chunk)>();
	} else {
		LOG_INFO << "%13c\twith master : " << symbol.master()->name();
		WriteSymbolRefBody(zone.acquireMemory<sizeof(bytecode::Chunk)>("parent"), *symbol.master(), helper);
	}
}
	
void ska::SerializerTypeTraits<ska::Symbol>::WriteSymbolRefBody(SerializerSafeZone<sizeof(bytecode::Chunk)> zone, const Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper) {
	auto [scriptId, operand] = helper.extractGeneratedOperandFromSymbol(symbol);

	//LOG_INFO << "%13c\twith Raw operand " << operand;
	//LOG_INFO << "%13c\twith " << symbol.size() << " children";

	// Operand (???)
	// OperandSerializer::write(*m_cache, buffer, natives, operand);

	auto absoluteScriptKey = helper.getAbsoluteScriptKey(scriptId, symbol);

	LOG_INFO << "%13cSymbol Name refered : " << symbol.name() << " with key : " << absoluteScriptKey;

	zone.write(std::move(absoluteScriptKey));
}

