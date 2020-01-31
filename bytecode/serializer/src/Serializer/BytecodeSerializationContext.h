#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include "NodeValue/ASTNodePtr.h"

#include "BytecodeSerializationContext.h"
#include "Generator/Value/BytecodeScriptCache.h"

namespace ska {
	namespace bytecode {
		struct SerializationContext {
			SerializationContext(const ScriptCache& cache, const std::size_t& generated, std::ostream& output) :
				m_id(generated),
				m_output(output),
				m_cache(cache) {
			}

			auto begin() const { return m_cache[m_id].begin(); }
			auto end() const { return m_cache[m_id].end(); }

			const auto& exports() const { return m_cache[m_id].exportedSymbols(); }

			bool next() { m_id++; return m_id < m_cache.size(); }

			const std::string& currentScriptName() const { return m_cache[m_id].name(); }
			std::size_t currentScriptId() const { return m_id; }

			template <class T>
			std::ostream& operator<<(const T& value) {
				m_output << value;
				return m_output;
			}

		private:
			const ScriptCache& m_cache;
			std::size_t m_id;
			std::ostream& m_output;
		};

	}
}
