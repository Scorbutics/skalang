#include "Serializer/Config/LoggerSerializer.h"
#include "BytecodeSymbolTableSerializer.h"
#include "Serializer/BytecodeCommonSerializer.h"
#include "Serializer/BytecodeOperandSerializer.h"
#include "Generator/Value/BytecodeScriptCache.h"

#include "SerializerSymbol.h"
#include "SerializerSymbolizedType.h"

SKA_LOGC_CONFIG(ska::LogLevel::Info, ska::bytecode::SymbolTableSerializer);

#define LOG_DEBUG SLOG_STATIC(ska::LogLevel::Debug, ska::bytecode::SymbolTableSerializer)
#define LOG_INFO SLOG_STATIC(ska::LogLevel::Info, ska::bytecode::SymbolTableSerializer)

ska::bytecode::SymbolTableSerializer::SymbolTableSerializer(const ScriptCache& cache) :
	m_helper(cache) {
}

void ska::bytecode::SymbolTableSerializer::writeFull(SerializerOutput output, std::size_t id) {
	m_helper.getMapBuilder(id).write(output, *this);
	output.validateOrThrow();
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
	
	auto symbolSerializer = SerializerType<Symbol, SymbolTableSerializerHelper&>{ output };
	symbolSerializer.write(*value, m_helper);
}

void ska::bytecode::SymbolTableSerializer::write(SerializerOutput& output, const Symbol* symbol, const Type& type) {
	auto symbolizedTypeSerializer = SerializerType<CSymbolizedType, SymbolTableSerializerHelper&>{ output };
	symbolizedTypeSerializer.write(CSymbolizedType { type, symbol }, m_helper);
	
	for (auto& childType : type) {
		LOG_INFO << "\t\tChild type " << childType;
		LOG_INFO << "%13c\t\twith Raw type : " << ExpressionTypeSTR[static_cast<std::size_t>(childType.type())];
		LOG_INFO << "%13c\t\twith " << childType.size() << " children";

		// Recursive call into this function is done inside Type::serialize
		childType.serialize(output, *this, true);
	}
}
