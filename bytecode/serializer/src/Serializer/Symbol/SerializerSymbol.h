#pragma once

#include "Base/Serialization/SerializerType.h"
#include "NodeValue/Symbol.h"
#include "Serializer/BytecodeChunk.h"

namespace ska {
	namespace bytecode {
		class SymbolTableDeserializerHelper;
		class SymbolTableSerializerHelper;
	}

	template <>
	struct SerializerTypeTraits<Symbol*> {
		static constexpr std::size_t BytesRequired = 4 * sizeof(bytecode::Chunk) + sizeof(uint8_t);
		static constexpr const char* Name = "Symbol";

		static void Read(SerializerSafeZone<BytesRequired>& zone, Symbol*& symbol, bytecode::SymbolTableDeserializerHelper& helper);
		static void Write(SerializerSafeZone<BytesRequired>& zone, const Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper);

	private:
		static void WriteSymbolRefBody(SerializerSafeZone<2*sizeof(bytecode::Chunk)> zone, const Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper);
	};
}
