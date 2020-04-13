#include "Config/LoggerConfigLang.h"
#include "SerializerSymbolizedType.h"
#include "NodeValue/Symbol.h"

#include "SerializerSymbol.h"

namespace ska {
	struct SerializerTypeTraitsSymbolizedTypeLog;
}

SKA_LOGC_CONFIG(ska::LogLevel::Disabled, ska::SerializerTypeTraitsSymbolizedTypeLog);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::SerializerTypeTraitsSymbolizedTypeLog)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::SerializerTypeTraitsSymbolizedTypeLog)

void ska::SerializerTypeTraits<ska::SymbolizedType>::Read(SerializerSafeZone<BytesRequired>& zone, SymbolizedType& symbolizedType, bytecode::SymbolTableDeserializerHelper& helper) {
	auto rawType = static_cast<ExpressionType>(zone.read<uint8_t>());
	symbolizedType.type = rawType;

	auto hasSymbol = zone.read<uint8_t>();
	if (hasSymbol) {
		auto symbolSerializer = SerializerType<Symbol*, bytecode::SymbolTableDeserializerHelper&>{ zone };
		symbolSerializer.read(symbolizedType.symbol, helper);
	} else {
		static constexpr auto BytesSymbol = SerializerType<Symbol*, bytecode::SymbolTableDeserializerHelper&>::BytesRequired;
		zone.acquireMemory<BytesSymbol>("No symbol (byte padding)").readNull<BytesSymbol>();
	}
	
	symbolizedType.compoundTypes = zone.read<uint32_t>();

	if (symbolizedType.symbol != nullptr) {
		symbolizedType.type = Type::Override(symbolizedType.type, symbolizedType.symbol);
		symbolizedType.symbol->changeTypeIfRequired(symbolizedType.type);
	}

	LOG_INFO << "Type \"" << symbolizedType.type << "\" is being deserialized with " << symbolizedType.type.size() << " compound types";
}

void ska::SerializerTypeTraits<ska::CSymbolizedType>::Write(SerializerSafeZone<BytesRequired>& zone, const CSymbolizedType& symbolizedType, bytecode::SymbolTableSerializerHelper& helper) {
	auto rawType = symbolizedType.type.type();
	zone.write(static_cast<uint8_t>(rawType));

	LOG_INFO << "Type \"" << symbolizedType.type << "\" is being serialized with " << symbolizedType.type.size() << " compound types";

	if (symbolizedType.symbol != nullptr) {
		zone.write(static_cast<uint8_t>(1));
		auto symbolSerializer = SerializerType<Symbol*, bytecode::SymbolTableSerializerHelper&>{ zone };
		symbolSerializer.write(*symbolizedType.symbol, helper);
	} else {
		zone.write(static_cast<uint8_t>(0));
		static constexpr auto BytesSymbol = SerializerType<Symbol*, bytecode::SymbolTableSerializerHelper&>::BytesRequired;
		zone.acquireMemory<BytesSymbol>("No symbol (byte padding)").writeNull<BytesSymbol>();
	}

	zone.write(static_cast<uint32_t>(symbolizedType.type.size()));
}
