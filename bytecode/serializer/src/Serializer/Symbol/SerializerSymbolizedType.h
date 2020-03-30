#pragma once

#include "Base/Serialization/SerializerType.h"
#include "BytecodeSymbolTableSerializerHelper.h"
#include "NodeValue/Type.h"
#include "Serializer/BytecodeChunk.h"

namespace ska {
	class Symbol;

	namespace bytecode {
		class SymbolTableSerializerHelper;
	}

	struct CSymbolizedType {
		const Type& type;
		const Symbol* symbol = nullptr;
	};
	
	struct SymbolizedType {
		Type& type;
		Symbol* symbol = nullptr;
	};
	
	template <>
	struct SerializerTypeTraits<CSymbolizedType> {
		static constexpr std::size_t BytesRequired = sizeof(uint8_t) + sizeof(uint32_t) + 2 * sizeof(bytecode::Chunk);
		static constexpr const char* Name = "CSymbolizedType";

		static void Read(SerializerSafeZone<BytesRequired>& zone, CSymbolizedType& symbolizedType, bytecode::SymbolTableSerializerHelper&) {}
		static void Write(SerializerSafeZone<BytesRequired>& zone, const CSymbolizedType& symbolizedType, bytecode::SymbolTableSerializerHelper&);

	private:
		static void WriteTypeAndSymbolOneLevel(SerializerSafeZone<BytesRequired>& zone, const CSymbolizedType& symbol, bytecode::SymbolTableSerializerHelper&);
	};
	
	template <>
	struct SerializerTypeTraits<SymbolizedType> {
		static constexpr std::size_t BytesRequired = SerializerTypeTraits<CSymbolizedType>::BytesRequired;
		static constexpr const char* Name = "SymbolizedType";

		static void Read(SerializerSafeZone<BytesRequired>& zone, SymbolizedType& symbolizedType, bytecode::SymbolTableSerializerHelper&);
		static void Write(SerializerSafeZone<BytesRequired>& zone, const SymbolizedType& symbolizedType, bytecode::SymbolTableSerializerHelper&){}
	};
}
