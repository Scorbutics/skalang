#pragma once
#include <string>

namespace ska {

	enum class SymbolTableLookupType {
		Hierarchical,
		Direct
	};

	struct SymbolTableLookup {
	private:
		SymbolTableLookup(SymbolTableLookupType type, std::string symbol) :
			type(std::move(type)),
			symbolName(std::move(symbol)) {}

	public:
		static SymbolTableLookup direct(std::string symbol) {
			return {SymbolTableLookupType::Direct, std::move(symbol) };
		}

		static SymbolTableLookup hierarchical(std::string symbol) {
			return {SymbolTableLookupType::Hierarchical, std::move(symbol) };
		}

		SymbolTableLookupType type;
		std::string symbolName;
	};

	struct SymbolTableNested {
	private:
		SymbolTableNested(int depth, std::size_t childIndex) :
			depth(depth),
			childIndex(childIndex) {}

	public:
		static SymbolTableNested lastChild(std::size_t depth = 1) {
			return {static_cast<int>(depth), std::numeric_limits<std::size_t>::max() };
		}

		static SymbolTableNested firstChild(std::size_t depth = 1) {
			return {static_cast<int>(depth), 0 };
		}

		static SymbolTableNested parent(std::size_t depth = 1) {
			return {- static_cast<int>(depth), 0 };
		}

		static SymbolTableNested current() {
			return { 0, 0 };
		}

		int depth = 0;
		std::size_t childIndex = 0;
	};
}
