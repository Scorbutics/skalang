#pragma once
#include <sstream>
#include <memory>
#include <optional>
#include <vector>
#include <unordered_set>
#include <cstdint>
#include "NodeValue/ASTNodePtr.h"

#include "BytecodeSerializationStrategy.h"
#include "BytecodeScriptParts.h"
#include "BytecodeChunk.h"
#include "Generator/Value/BytecodeExport.h"
#include "Generator/Value/BytecodeScriptCache.h"
#include "Serializer/Symbol/BytecodeSymbolTableDeserializer.h"

namespace ska {
	namespace bytecode {
		struct ScriptHeader;
		struct ScriptBody;
		struct ScriptExternalReferences;

		struct DeserializationContext {
			DeserializationContext(ScriptCache& cache, std::string scriptStartName, DeserializationStrategy strategy) :
				m_strategy(std::move(strategy)),
				m_scriptStartName(std::move(scriptStartName)),
				m_cache(cache),
				m_symbolsDeserializer(cache) {
			}

			void declare(std::string scriptName, std::vector<Instruction> instructions, std::vector<ExportSymbol> exports);
			bool read(const std::string& scriptName) {
				try { m_input = &m_strategy(scriptName); return !m_input->eof(); }
				catch (std::runtime_error&) { return false; }
			}

			const std::string& startScriptName() const { return m_scriptStartName; }

			void operator>>(ScriptHeader& header);
			void operator>>(ScriptBody& body);
			void operator>>(ScriptExternalReferences& externalReferences);
			void operator>>(SerializerNativeContainer& natives);
		private:
			void replaceAllNativesRef(std::vector<ExportSymbol>& operands, const SerializerNativeContainer& natives) const;
			void replaceAllNativesRef(std::vector<Instruction>& instructions, const SerializerNativeContainer& natives) const;
			void replaceAllNativesRef(Operand& operand, const SerializerNativeContainer& natives) const;

			void operator>>(std::size_t& value);
			void operator>>(Chunk& value);
			void operator>>(Instruction& value);
			void operator>>(Operand& value);

			std::string readString();
			std::vector<ExportSymbol> readExports();
			std::unordered_set<std::string> readLinkedScripts(const SerializerNativeContainer& natives);
			std::vector<Instruction> readInstructions();
			void readSymbolTable(SerializerNativeContainer& natives);
			void checkValidity() const { if (m_input == nullptr) { throw std::runtime_error("no input available"); } }

			ScriptCache& m_cache;
			std::string m_scriptStartName;
			DeserializationStrategy m_strategy;
			std::istream* m_input = nullptr;
			SymbolTableDeserializer m_symbolsDeserializer;
		};

	}
}
