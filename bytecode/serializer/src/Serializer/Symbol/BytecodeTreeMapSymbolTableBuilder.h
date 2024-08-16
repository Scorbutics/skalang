#pragma once
#include <map>
#include <optional>
#include <unordered_map>
#include "Generator/Value/BytecodeSymbolInfo.h"

namespace ska {
	class SymbolTable;
	class Symbol;
	class ScopedSymbolTable;

	namespace bytecode {
		class ScriptCache;

		class TreeMapSymbolTableBuilder {
			using IndexSymbolMap = std::unordered_map<const Symbol*, std::string>;
		public:
			using ReverseIndexSymbolMapRead = std::map<std::string, ScopedSymbolTable*>;

			TreeMapSymbolTableBuilder(SymbolTable& table);

			TreeMapSymbolTableBuilder(TreeMapSymbolTableBuilder&&) noexcept = default;
			TreeMapSymbolTableBuilder& operator=(TreeMapSymbolTableBuilder&&) = default;
			TreeMapSymbolTableBuilder(const TreeMapSymbolTableBuilder&) = delete;
			TreeMapSymbolTableBuilder& operator=(const TreeMapSymbolTableBuilder&) = delete;

			~TreeMapSymbolTableBuilder() = default;


			ScopedSymbolTable* value(const std::string& key, std::optional<std::string> name);

		private:
			ScopedSymbolTable& walkScope(const std::vector<std::string>& parts, std::string leafName);

			ReverseIndexSymbolMapRead m_symbolsReversedRead;
			ScopedSymbolTable* m_rootRead;
		};
	}
}
