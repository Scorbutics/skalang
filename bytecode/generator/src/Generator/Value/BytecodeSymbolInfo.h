#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <ostream>
#include "Runtime/Value/StringShared.h"
#include "Runtime/Value/ScriptVariableRef.h"
#include "Runtime/Value/NativeFunction.h"

namespace ska {
	namespace bytecode {
		using FieldsReferencesRaw = std::unordered_map<ScriptVariableRef, std::size_t>;
		using FieldsReferences = std::shared_ptr<FieldsReferencesRaw>;

		struct SymbolInfo {
			SymbolInfo() = delete;

			StringShared name;
			FieldsReferences references;
			bool exported = false;
			std::size_t script = static_cast<std::size_t>(-1);
			std::size_t priority = 0;
			NativeFunctionPtr binding = nullptr;

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
			SymbolInfo(std::size_t scriptIndex) : script(scriptIndex) {}

			bool empty() const { return name == nullptr || name->empty(); }
		};

		using SymbolInfoPack = std::vector<SymbolInfo>;

		std::ostream& operator<<(std::ostream& stream, const SymbolInfo& symbol);
		std::ostream& operator<<(std::ostream& stream, const SymbolInfoPack& symbols);
	}
}
