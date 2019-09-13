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
			bool exported = false;
			std::size_t script = static_cast<std::size_t>(-1);
			std::size_t priority = 0;

			SymbolInfo(std::size_t scopeIndex, std::string name, std::size_t scriptIndex) :
				name(std::make_shared<std::string>(scopeIndex == 1 ? std::move(name) : "")),
				script(scriptIndex)
				{}
			SymbolInfo(FieldsReferences refs, std::size_t scriptIndex) :
				references(std::move(refs)),
				script(scriptIndex) {}

			SymbolInfo(std::size_t scopeIndex, std::string name, FieldsReferences refs, std::size_t scriptIndex) :
				SymbolInfo(scopeIndex, name, scriptIndex) {
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
