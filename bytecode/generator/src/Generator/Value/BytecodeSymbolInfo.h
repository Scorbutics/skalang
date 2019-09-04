#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <ostream>
#include "NodeValue/StringShared.h"

namespace ska {
	namespace bytecode {
		using FieldsReferencesRaw = std::unordered_map<std::size_t, std::size_t>;
		using FieldsReferences = std::shared_ptr<FieldsReferencesRaw>;

		struct SymbolInfo {
			StringShared name;
			FieldsReferences references;

			SymbolInfo(std::size_t scopeIndex, std::string name) :
				name(std::make_shared<std::string>(scopeIndex == 1 ? std::move(name) : ""))
				{}
			SymbolInfo(FieldsReferences refs) :
				references(std::move(refs)) {}

			SymbolInfo(std::size_t scopeIndex, std::string name, FieldsReferences refs) :
				SymbolInfo(scopeIndex, name) {
				references = std::move(refs);
			}

			SymbolInfo() = default;

			bool empty() const { return name == nullptr || name->empty(); }
		};

		using SymbolInfoPack = std::vector<SymbolInfo>;

		std::ostream& operator<<(std::ostream& stream, const SymbolInfo& symbol);
		std::ostream& operator<<(std::ostream& stream, const SymbolInfoPack& symbols);
	}
}
