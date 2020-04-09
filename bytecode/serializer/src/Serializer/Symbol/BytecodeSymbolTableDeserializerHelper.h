#pragma once
#include <memory>
#include <unordered_map>
#include "Serializer/Symbol/BytecodeTreeMapSymbolTableBuilder.h"
#include "Generator/Value/BytecodeOperand.h"
#include "Base/Serialization/SerializerSafeZone.h"

namespace ska {

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
			Symbol& buildSymbol(detail::SerializerSafeZone& zone, const std::string& absoluteScriptKey, std::string symbolName, const Operand& operand);
			void buildFieldReferences();
			Operand readOperand(SerializerSafeZone<17> safeZone);

		public:
			ScriptCache* m_cache = nullptr;
			std::unordered_map<std::string, std::unique_ptr<TreeMapSymbolTableBuilder>> m_symbolTableBuilder;
		};
	}
}
