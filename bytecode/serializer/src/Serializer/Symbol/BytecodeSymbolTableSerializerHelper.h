#pragma once
#include <optional>
#include <vector>
#include "Serializer/Symbol/BytecodeTreeSymbolTableMapBuilder.h"
#include "Generator/Value/BytecodeOperand.h"

namespace ska {
	namespace bytecode {
		class ScriptCache;

		class SymbolTableSerializerHelper {
		public:
			SymbolTableSerializerHelper(const ScriptCache& cache);

			SymbolTableSerializerHelper(SymbolTableSerializerHelper&&) = delete;
			SymbolTableSerializerHelper(const SymbolTableSerializerHelper&) = delete;
			SymbolTableSerializerHelper& operator=(SymbolTableSerializerHelper&&) = delete;
			SymbolTableSerializerHelper& operator=(const SymbolTableSerializerHelper&) = delete;
			~SymbolTableSerializerHelper() = default;

		
			TreeSymbolTableMapBuilder& getMapBuilder(std::size_t id);
			std::string getAbsoluteScriptKey(std::size_t scriptId, const Symbol& value);

			std::pair<std::size_t, Operand> extractGeneratedOperandFromSymbol(const Symbol& symbol);

		public:
			const ScriptCache* m_cache = nullptr;
			std::vector<std::optional<TreeSymbolTableMapBuilder>> m_mapBuilder;
		};
	}
}
