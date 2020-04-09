#pragma once

#include "Base/Serialization/SerializerType.h"
#include "NodeValue/Symbol.h"
#include "Serializer/BytecodeChunk.h"

namespace ska {
	namespace bytecode {
		class SymbolTableDeserializerHelper;
		class SymbolTableSerializerHelper;
	}

	template <std::size_t Bytes>
	struct BytesValue {
		static constexpr auto value = Bytes;
	};

	template <>
	struct SerializerTypeTraits<Symbol*> {
		static constexpr std::size_t BytesSymbolRefRequired = 4 * sizeof(bytecode::Chunk) + sizeof(uint8_t);
		static constexpr std::size_t BytesRequired = 2 * BytesValue<BytesSymbolRefRequired>::value + sizeof(uint8_t);

		static constexpr const char* Name = "Symbol";

		static void Read(SerializerSafeZone<BytesRequired>& zone, Symbol*& symbol, bytecode::SymbolTableDeserializerHelper& helper);
		static void Write(SerializerSafeZone<BytesRequired>& zone, const Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper);

	private:
		static Symbol& ReadSymbolRefBody(SerializerSafeZone<BytesSymbolRefRequired> zone, bytecode::SymbolTableDeserializerHelper& helper);
		static void WriteSymbolRefBody(SerializerSafeZone<BytesSymbolRefRequired> zone, const Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper);
	};
}
