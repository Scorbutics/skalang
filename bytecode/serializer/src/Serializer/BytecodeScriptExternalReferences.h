#pragma once
#include <cstddef>
#include <unordered_set>
#include "BytecodeChunk.h"

namespace ska {
	namespace bytecode {
		struct ScriptHeader;
		struct ScriptBody;

		struct ScriptExternalReferences {
			ScriptExternalReferences(const std::vector<std::string>& natives) : m_natives(natives) {}
			const std::vector<std::string>& natives() const { return m_natives; }
			std::unordered_set<std::string> scripts{};
		private:
			const std::vector<std::string>& m_natives;
		};
	}
}
