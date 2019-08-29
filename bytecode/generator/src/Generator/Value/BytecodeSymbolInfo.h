#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <ostream>

namespace ska {
	namespace bytecode {
		using FieldsReferencesRaw = std::unordered_map<std::size_t, std::size_t>;
		using FieldsReferences = std::shared_ptr<FieldsReferencesRaw>;

		struct SymbolInfo {
			std::string name;
			FieldsReferences references;

			SymbolInfo(std::size_t scopeIndex, std::string name) :
				name(scopeIndex == 1 ? std::move(name) : "")
				{}
			SymbolInfo(FieldsReferences refs) :
				references(std::move(refs)) {}

			SymbolInfo() = default;

			bool empty() const { return name.empty(); }
		};

		using SymbolInfoPack = std::vector<SymbolInfo>;

		std::ostream& operator<<(std::ostream& stream, const SymbolInfo& symbol);
		std::ostream& operator<<(std::ostream& stream, const SymbolInfoPack& symbols);
	}
}
