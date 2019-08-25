#pragma once
#include <vector>
#include <ostream>

namespace ska {
	namespace bytecode {
		struct SymbolInfo {
			std::string name;

			SymbolInfo(std::size_t scopeIndex, std::string name) : name(scopeIndex == 1 ? std::move(name) : "") {}
			SymbolInfo() = default;

			bool empty() const { return name.empty(); }
		};
		
		using SymbolInfoPack = std::vector<SymbolInfo>;
		
		std::ostream& operator<<(std::ostream& stream, const SymbolInfo& symbol);
		std::ostream& operator<<(std::ostream& stream, const SymbolInfoPack& symbols);
	}
}
