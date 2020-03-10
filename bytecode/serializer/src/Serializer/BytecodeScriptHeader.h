#pragma once
#include <cstddef>

#include "BytecodeChunk.h"

namespace ska {
	namespace bytecode {
		struct ScriptHeader {
			ScriptHeader(const std::vector<std::string>& natives) : m_natives(natives) {}

			const std::string& scriptName() {
				if (m_scriptName.empty()) { if (static_cast<std::size_t>(scriptNameRef) >= m_natives.size()) { throw std::runtime_error("no script name present in natives"); } m_scriptName = m_natives[scriptNameRef]; }
				return m_scriptName;
				 
			}

			const std::vector<std::string>& m_natives;
			Chunk scriptNameRef { 0 };
			std::string m_scriptName {};
			std::size_t serializerVersion { 0 };
			std::size_t scriptBridged { 0 };
			std::size_t scriptId { 0 };
		};
	}
}
