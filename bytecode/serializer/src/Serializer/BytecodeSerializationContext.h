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
				m_output(&m_strategy(cache[m_id].name())),
				m_buffer(1) {
			}

			bool next() {
				pushNatives();
				commit();
				m_id++;
				if (m_id < m_cache.size()) {
					m_output = &m_strategy(m_cache[m_id].name());
					return true;
				}
				return false;
			}

			bool currentScriptBridged() const { return m_cache[m_id].program().isBridged(); }

			void commit();
			std::stringstream pop();
			void push(std::stringstream data);

			void writeHeader(std::size_t serializerVersion);
			std::vector<std::string> writeInstructions();
			void writeExports();
			void writeExternalReferences(std::vector<std::string> linkedScripts);

		private:
			auto& buffer() { return m_buffer[m_bufferIndex]; }

			const std::string& currentScriptName() const { return m_cache[m_id].name(); }
			const std::string scriptName(std::size_t id) const { return m_cache[id].name(); }
			std::size_t currentScriptId() const { return m_id; }
			
			void pushNatives();
			auto begin() const { return m_cache[m_id].begin(); }
			auto end() const { return m_cache[m_id].end(); }

			void operator<<(std::size_t value);
			void operator<<(std::string value);
			void operator<<(const Instruction& value);
			void operator<<(const Operand& value);

			const ScriptCache& m_cache;
			std::size_t m_id = 0;
			std::size_t m_bufferIndex = 0;
			SerializationStrategy m_strategy;
			std::vector<std::string> m_natives;
			std::vector<std::stringstream> m_buffer;
			std::ostream* m_output = nullptr;
		};

	}
}
