#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include <cstdint>
#include "NodeValue/ASTNodePtr.h"

#include "BytecodeSerializationStrategy.h"
#include "BytecodeChunk.h"
#include "Generator/Value/BytecodeScriptCache.h"

namespace ska {
	namespace bytecode {

		struct SerializationContext {
			SerializationContext(const ScriptCache& cache, SerializationStrategy strategy) :
				m_strategy(std::move(strategy)),
				m_cache(cache),
				m_output(&m_strategy(cache[m_id].name())) {
			}

			bool next() {
				pushNatives();
				m_id++;
				if (m_id < m_cache.size()) {
					m_output = &m_strategy(m_cache[m_id].name());
					return true;
				}
				return false;
			}

			bool currentScriptBridged() const { return m_cache[m_id].program().isBridged(); }

			void writeHeader(std::size_t serializerVersion);
			std::vector<std::string> writeInstructions();
			void writeExports();
			void writeExternalReferences(std::vector<std::string> linkedScripts);

		private:
			const std::string& currentScriptName() const { return m_cache[m_id].name(); }
			const std::string scriptName(std::size_t id) const { return m_cache[id].name(); }
			std::size_t currentScriptId() const { return m_id; }
			void pushNatives();

			auto begin() const { return m_cache[m_id].begin(); }
			auto end() const { return m_cache[m_id].end(); }

			std::ostream& operator<<(std::size_t value);
			std::ostream& operator<<(std::string value);
			std::ostream& operator<<(const Instruction& value);
			std::ostream& operator<<(const Operand& value);

			const ScriptCache& m_cache;
			std::size_t m_id = 0;
			SerializationStrategy m_strategy;
			std::vector<std::string> m_natives;
			std::ostream* m_output = nullptr;
		};

	}
}
