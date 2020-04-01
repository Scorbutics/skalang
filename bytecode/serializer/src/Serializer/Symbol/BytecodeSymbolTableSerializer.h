#pragma once
#include <string>
#include <utility>
#include <sstream>
#include <optional>
#include "BytecodeTreeSymbolTableMapBuilder.h"
#include "BytecodeSymbolTableSerializerHelper.h"

#include "Generator/Value/BytecodeOperand.h"
#include "Runtime/Service/ScriptTypeSerializer.h"
#include "Base/Serialization/SerializerSafeZone.h"
#include "Serializer/BytecodeChunk.h"

namespace ska {
	class Symbol;
	class SerializerOutput;

	namespace bytecode {
		class ScriptCache;

		class SymbolTableSerializer :
			public ScriptTypeSerializer {
		public:
			SymbolTableSerializer(const ScriptCache& cache);
			SymbolTableSerializer(SymbolTableSerializer&&) = delete;
			SymbolTableSerializer(const SymbolTableSerializer&) = delete;
			SymbolTableSerializer& operator=(SymbolTableSerializer&&) = delete;
			SymbolTableSerializer& operator=(const SymbolTableSerializer&) = delete;
			~SymbolTableSerializer() = default;

			void writeIfExists(SerializerOutput& output, const Symbol* value);
			void writeFull(SerializerOutput output, std::size_t id);
			void writeFull(SerializerOutput& output, const TreeSymbolTableMapBuilder::ReverseIndexSymbolMapWrite& reversedMap);		

			void write(SerializerOutput& output, const Symbol* value, const Type& type) override;
		private:
			SymbolTableSerializerHelper m_helper;
		};
	}
}
