#pragma once
#include <string>
#include <utility>
#include <sstream>
#include <optional>
#include "BytecodeTreeSymbolTableMapBuilder.h"

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
			void writeFull(SerializerOutput& output, const TreeSymbolTableMapBuilder::ReverseIndexSymbolMap& reversedMap);

			void write(SerializerOutput& output, const Symbol* value, const Type& type) override;			
		private:
			TreeSymbolTableMapBuilder& getMapBuilder(std::size_t id);
			std::string getAbsoluteScriptKey(std::size_t scriptId, const Symbol& value);

			void writeSymbolRefAndParents(SerializerSafeZone<2 * sizeof(Chunk)> output, const Symbol& value);
			void writeSymbolRefBody(SerializerSafeZone<sizeof(Chunk)> output, const Symbol& value);
			void writeTypeAndSymbolOneLevel(SerializerSafeZone<sizeof(uint32_t) + sizeof(uint8_t) + 2 * sizeof(Chunk)> output, const Symbol* symbol, const Type& type);

			std::pair<std::size_t, Operand> extractGeneratedOperandFromSymbol(const Symbol& symbol);

			const ScriptCache* m_cache = nullptr;
			std::vector<std::optional<TreeSymbolTableMapBuilder>> m_mapBuilder;
		};
	}
}
