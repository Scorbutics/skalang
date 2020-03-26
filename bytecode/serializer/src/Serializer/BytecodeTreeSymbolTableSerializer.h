#pragma once
#include <unordered_map>

namespace ska {
	class SymbolTable;
	class Symbol;
	class ScopedSymbolTable;

	namespace bytecode {
		class TreeSymbolTableSerializer {
			using IndexSymbolMap = std::unordered_map<const Symbol*, std::string>;
		public:
			TreeSymbolTableSerializer() = default;
			~TreeSymbolTableSerializer() = default;

			void serialize(const SymbolTable& table);
			void serialize(const ScopedSymbolTable& table, const std::string& depth);
			void serialize(const Symbol& symbol, const std::string& depth, std::size_t childIndex);

		private:
			static std::string buildKey(const std::string& depth, std::size_t childIndex);
			IndexSymbolMap m_symbols;
		};
	}
}
