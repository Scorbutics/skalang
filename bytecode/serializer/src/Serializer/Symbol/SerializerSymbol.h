#pragma once

#include "Base/Serialization/SerializerType.h"
#include "BytecodeSymbolTableSerializerHelper.h"
#include "NodeValue/Symbol.h"
#include "Serializer/BytecodeChunk.h"

namespace ska {
	template <>
	struct SerializerTypeTraits<Symbol> {
		static constexpr std::size_t BytesRequired = 2 * sizeof(bytecode::Chunk);
		static constexpr const char* Name = "Symbol";

		static void Read(SerializerSafeZone<BytesRequired>& zone, Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper);
		static void Write(SerializerSafeZone<BytesRequired>& zone, const Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper);

	private:
		static void WriteSymbolRefBody(SerializerSafeZone<sizeof(bytecode::Chunk)> zone, const Symbol& symbol, bytecode::SymbolTableSerializerHelper& helper);
	};
}
