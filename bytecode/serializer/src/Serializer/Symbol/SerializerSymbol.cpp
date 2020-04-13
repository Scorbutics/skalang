#include "Config/LoggerConfigLang.h"
#include "SerializerSymbol.h"
#include "BytecodeSymbolTableDeserializer.h"
#include "BytecodeSymbolTableSerializer.h"

namespace ska {
	struct SerializerTypeTraitsSymbolLog;
}

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::SerializerTypeTraitsSymbolLog);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::SerializerTypeTraitsSymbolLog)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::SerializerTypeTraitsSymbolLog)

void ska::SerializerTypeTraits<ska::Symbol*>::Read(SerializerSafeZone<BytesRequired>& zone, Symbol*& symbol, bytecode::SymbolTableDeserializerHelper& helper) {
	symbol = &ReadSymbolRefBody(zone.acquireMemory<BytesSymbolRefRequired>("Symbol ref"), helper);

	const auto hasMaster = zone.read<uint8_t>() != 0;
	if (!hasMaster) {
		LOG_INFO << "%13cNo parent symbol";
		auto safeNullZone = zone.acquireMemory<BytesSymbolRefRequired>("No parent");
		safeNullZone.readNull<BytesSymbolRefRequired>();
	} else {
		LOG_INFO << "%13c\twith master : ";
		auto& master = ReadSymbolRefBody(zone.acquireMemory<BytesSymbolRefRequired>("Master symbol ref"), helper);
		master.implement(*symbol);
	}
}

ska::Symbol& ska::SerializerTypeTraits<ska::Symbol*>::ReadSymbolRefBody(SerializerSafeZone<BytesSymbolRefRequired> zone, bytecode::SymbolTableDeserializerHelper& helper) {
	auto absoluteScriptKey = zone.read<std::string>();
	auto symbolName = zone.read<std::string>();
	LOG_INFO << "%13c" << symbolName << " with key : " << absoluteScriptKey;
	auto operand = helper.readOperand(zone.acquireMemory<sizeof(uint8_t) + sizeof(bytecode::Chunk) * 2>("symbol operand"));
	return helper.buildSymbol(zone, absoluteScriptKey, symbolName, operand);
}

void ska::SerializerTypeTraits<ska::Symbol*>::Write(SerializerSafeZone<BytesRequired>& zone, const Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper) {
	WriteSymbolRefBody(zone.acquireMemory<BytesSymbolRefRequired>("Symbol ref"), symbol, helper);

	const auto hasMaster = &symbol != symbol.master();
	zone.write(static_cast<uint8_t>(hasMaster));
	if (!hasMaster) {
		zone.writeNull<BytesSymbolRefRequired>();
	} else {
		LOG_INFO << "%13c\twith master : " << symbol.master()->name();
		WriteSymbolRefBody(zone.acquireMemory<BytesSymbolRefRequired>("parent"), *symbol.master(), helper);
	}
}
	
void ska::SerializerTypeTraits<ska::Symbol*>::WriteSymbolRefBody(SerializerSafeZone<BytesSymbolRefRequired> zone, const Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper) {
	auto scriptId = helper.scriptOfSymbol(symbol);
	auto operand = helper.operandOfSymbol(symbol);
	auto relativeScriptKey = helper.getRelativeScriptKey(scriptId, symbol);
	auto absoluteScriptKey = std::to_string(zone.ref(helper.getScriptName(scriptId))) + (relativeScriptKey.empty() ? "" : ("." + relativeScriptKey));

	LOG_INFO << "%13cSymbol Name refered : " << symbol.name() << " with key : " << absoluteScriptKey;

	zone.write(std::move(absoluteScriptKey));
	zone.write(symbol.name());
	helper.writeOperand(zone.acquireMemory<sizeof(uint8_t) + sizeof(bytecode::Chunk) * 2>("symbol operand"), operand);
}

