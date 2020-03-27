#pragma once
#include <unordered_map>
#include <map>
#include <sstream>

namespace ska {
	class SymbolTable;
	class Symbol;
	class ScopedSymbolTable;

	namespace bytecode {
		class SymbolTableSerializer;

		class TreeSymbolTableMapBuilder {
			using IndexSymbolMap = std::unordered_map<const Symbol*, std::string>;
		public:
			using ReverseIndexSymbolMap = std::map<std::string, const Symbol*>;
			TreeSymbolTableMapBuilder(const SymbolTable& table);
			~TreeSymbolTableMapBuilder() = default;

			std::string TreeSymbolTableMapBuilder::key(const Symbol& symbol) const;
			void write(std::stringstream& buffer, std::unordered_map<std::string, std::size_t>& natives, SymbolTableSerializer& serializer) const;

		private:
			void store(const SymbolTable& table, const std::string& defaultDepth);
			void store(const ScopedSymbolTable& table, const std::string& depth);
			void store(const Symbol& symbol, const std::string& depth, std::size_t childIndex);

			static std::string buildKey(const std::string& depth, std::size_t childIndex);
			IndexSymbolMap m_symbols;
			ReverseIndexSymbolMap m_symbolsReversed;
		};
	}
}
