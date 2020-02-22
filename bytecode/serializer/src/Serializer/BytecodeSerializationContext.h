#pragma once
#include <sstream>
#include <memory>
#include <deque>
#include <unordered_set>
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

			bool next(std::deque<std::size_t> partIndexes) {
				partIndexes.push_front(pushNatives());
				commit(std::move(partIndexes));
				m_id++;
				if (m_cache.exist(m_id)) {
					m_output = &m_strategy(m_cache[m_id].name());
					return true;
				}
				return false;
			}

			bool currentScriptBridged() const { return m_cache[m_id].program().isBridged(); }

			std::size_t writeHeader(std::size_t serializerVersion);
			std::pair<std::size_t, std::vector<std::string>> writeInstructions();
			std::size_t writeExports();
			std::size_t writeExternalReferences(std::vector<std::string> linkedScripts);

		private:
			void commit(std::deque<std::size_t> partIndexes);
			void push();
			auto& buffer() { return m_buffer.back(); }

			const std::string& currentScriptName() const { return m_cache[m_id].name(); }
			const std::string scriptName(std::size_t id) const { return m_cache[id].name(); }
			std::size_t currentScriptId() const { return m_id; }
			
			std::size_t pushNatives();
			auto begin() const { return m_cache[m_id].begin(); }
			auto end() const { return m_cache[m_id].end(); }

			void operator<<(std::size_t value);
			void operator<<(std::string value);
			void operator<<(const Instruction& value);
			void operator<<(const Operand& value);

			const ScriptCache& m_cache;
			std::size_t m_id = 0;
			SerializationStrategy m_strategy;
			std::unordered_map<std::string, std::size_t> m_natives;
			std::vector<std::stringstream> m_buffer;
			std::ostream* m_output = nullptr;
		};

	}
}
