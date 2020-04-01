#pragma once
#include <memory>
#include <unordered_map>
#include "Serializer/Symbol/BytecodeTreeMapSymbolTableBuilder.h"
#include "Generator/Value/BytecodeOperand.h"

namespace ska {
	namespace detail {
		class SerializerSafeZone;
	}

	namespace bytecode {
		class ScriptCache;

		class SymbolTableDeserializerHelper {
		public:
			SymbolTableDeserializerHelper(ScriptCache& cache);

			SymbolTableDeserializerHelper(SymbolTableDeserializerHelper&&) = delete;
			SymbolTableDeserializerHelper(const SymbolTableDeserializerHelper&) = delete;
			SymbolTableDeserializerHelper& operator=(SymbolTableDeserializerHelper&&) = delete;
			SymbolTableDeserializerHelper& operator=(const SymbolTableDeserializerHelper&) = delete;
			~SymbolTableDeserializerHelper() = default;

		
			TreeMapSymbolTableBuilder& getSymbolTableBuilder(const std::string& scriptName);
			Symbol& buildSymbol(detail::SerializerSafeZone& zone, const std::string& absoluteScriptKey, std::string symbolName);			

		public:
			ScriptCache* m_cache = nullptr;
			std::unordered_map<std::string, std::unique_ptr<TreeMapSymbolTableBuilder>> m_symbolTableBuilder;
		};
	}
}
