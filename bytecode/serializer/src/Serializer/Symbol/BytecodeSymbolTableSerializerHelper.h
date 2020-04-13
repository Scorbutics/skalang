#pragma once
#include <memory>
#include <vector>
#include "Serializer/Symbol/BytecodeTreeSymbolTableMapBuilder.h"
#include "Base/Serialization/SerializerSafeZone.h"
#include "Generator/Value/BytecodeOperand.h"

namespace ska {
	namespace bytecode {
		class ScriptCache;

		class SymbolTableSerializerHelper {
		public:
			SymbolTableSerializerHelper(const ScriptCache& cache);

			SymbolTableSerializerHelper(SymbolTableSerializerHelper&&) = delete;
			SymbolTableSerializerHelper(const SymbolTableSerializerHelper&) = delete;
			SymbolTableSerializerHelper& operator=(SymbolTableSerializerHelper&&) = delete;
			SymbolTableSerializerHelper& operator=(const SymbolTableSerializerHelper&) = delete;
			~SymbolTableSerializerHelper() = default;

		
			TreeSymbolTableMapBuilder& getMapBuilder(std::size_t id);
			std::string getRelativeScriptKey(std::size_t scriptId, const Symbol& value);
			const std::string& getScriptName(const std::size_t scriptId) const;
			std::size_t scriptOfSymbol(const Symbol& symbol);
			Operand operandOfSymbol(const Symbol& symbol);
			void writeOperand(SerializerSafeZone<17> safeZone, const Operand& value);

		public:
			const ScriptCache* m_cache = nullptr;
			std::vector<std::unique_ptr<TreeSymbolTableMapBuilder>> m_mapBuilder;
		};
	}
}
