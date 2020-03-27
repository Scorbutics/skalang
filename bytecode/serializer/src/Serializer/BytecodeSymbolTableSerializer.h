#pragma once
#include <string>
#include <utility>
#include <sstream>
#include <optional>
#include "BytecodeTreeSymbolTableMapBuilder.h"

#include "Generator/Value/BytecodeOperand.h"

namespace ska {
	class Symbol;
	namespace bytecode {
		class ScriptCache;

		class SymbolTableSerializer {
		public:
			SymbolTableSerializer(const ScriptCache& cache);
			SymbolTableSerializer(SymbolTableSerializer&&) = delete;
			SymbolTableSerializer(const SymbolTableSerializer&) = delete;
			SymbolTableSerializer& operator=(SymbolTableSerializer&&) = delete;
			SymbolTableSerializer& operator=(const SymbolTableSerializer&) = delete;
			~SymbolTableSerializer() = default;

			void writeIfExists(std::stringstream& buffer, std::unordered_map<std::string, std::size_t>& natives, const Symbol* value);
			void writeFull(std::size_t id, std::unordered_map<std::string, std::size_t>& natives, std::stringstream& buffer);
			void writeFull(std::stringstream& buffer, std::unordered_map<std::string, std::size_t>& natives, const TreeSymbolTableMapBuilder::ReverseIndexSymbolMap& reversedMap);
		private:
			TreeSymbolTableMapBuilder& getMapBuilder(std::size_t id);
			std::string getAbsoluteScriptKey(std::size_t scriptId, const Symbol& value);
			void write(std::stringstream& buffer, std::unordered_map<std::string, std::size_t>& natives, const Type value);

			std::pair<std::size_t, Operand> extractGeneratedOperandFromSymbol(const Symbol& symbol);

			const ScriptCache* m_cache = nullptr;
			std::vector<std::optional<TreeSymbolTableMapBuilder>> m_mapBuilder;
		};
	}
}
