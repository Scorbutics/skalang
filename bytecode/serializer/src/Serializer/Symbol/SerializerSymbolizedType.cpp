#include "Config/LoggerConfigLang.h"
#include "SerializerSymbolizedType.h"
#include "BytecodeSymbolTableSerializer.h"
#include "NodeValue/Symbol.h"

#include "SerializerSymbol.h"

namespace ska {
	struct SerializerTypeTraitsSymbolizedTypeLog;
}

SKA_LOGC_CONFIG(ska::LogLevel::Debug, ska::SerializerTypeTraitsSymbolizedTypeLog);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::SerializerTypeTraitsSymbolizedTypeLog)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::SerializerTypeTraitsSymbolizedTypeLog)

void ska::SerializerTypeTraits<ska::SymbolizedType>::Read(SerializerSafeZone<BytesRequired>& zone, SymbolizedType& symbol, bytecode::SymbolTableSerializerHelper& helper) {
	//TODO
}

void ska::SerializerTypeTraits<ska::CSymbolizedType>::Write(SerializerSafeZone<BytesRequired>& zone, const CSymbolizedType& symbolizedType, bytecode::SymbolTableSerializerHelper& helper) {
	WriteTypeAndSymbolOneLevel(zone, symbolizedType, helper);
}
	
void ska::SerializerTypeTraits<ska::CSymbolizedType>::WriteTypeAndSymbolOneLevel(SerializerSafeZone<BytesRequired>& output, const CSymbolizedType& symbolizedType, bytecode::SymbolTableSerializerHelper& helper) {
	auto rawType = symbolizedType.type.type();
	output.acquireMemory<sizeof(uint8_t)>("raw").write(static_cast<uint8_t>(rawType));

	LOG_INFO << "Type \"" << symbolizedType.type << "\" is being serialized with " << symbolizedType.type.size() << " compound types";

	if (symbolizedType.symbol != nullptr) {
		auto symbolSerializer = SerializerType<Symbol, bytecode::SymbolTableSerializerHelper&>{ output };
		symbolSerializer.write(*symbolizedType.symbol, helper);
	} else {
		output.acquireMemory<2 * sizeof(bytecode::Chunk)>("no symbol ref & parent").writeNull<2 * sizeof(bytecode::Chunk)>();
	}

	output.acquireMemory<sizeof(uint32_t)>("type children").write(static_cast<uint32_t>(symbolizedType.type.size()));
}
