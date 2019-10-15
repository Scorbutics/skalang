#pragma once

#include <unordered_map>
#include <tuple>
#include <optional>
#include "BytecodeValue.h"

namespace ska {
  namespace bytecode {
    class UniqueSymbolGetterBase {
			using SymbolUIDContainer = std::unordered_map<const Symbol*, std::size_t>;
		public:
			virtual ~UniqueSymbolGetterBase() = default;

		protected:
			UniqueSymbolGetterBase(char symbol) : m_symbol(symbol) {}

			std::pair<Value, bool> query(std::size_t script, const ASTNode& node);
			std::pair<Value, bool> query(std::size_t script, const Symbol& symbol);
			std::optional<Value> get(std::size_t script, const Symbol& symbol) const;
				
			SymbolUIDContainer m_container;
			std::size_t m_count = 0;
			char m_symbol = '_';
		};

		template <char SymbolChar>
		class UniqueSymbolGetter :
			public UniqueSymbolGetterBase {

		public:
			UniqueSymbolGetter() : UniqueSymbolGetterBase(SymbolChar) {}
			~UniqueSymbolGetter() override = default;
		};
  }
}
