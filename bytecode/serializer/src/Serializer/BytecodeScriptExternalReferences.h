#pragma once
#include <cstddef>
#include <unordered_set>
#include "BytecodeChunk.h"

namespace ska {
	namespace bytecode {
		struct ScriptHeader;
		struct ScriptBody;

		struct ScriptExternalReferences {
			ScriptExternalReferences(ScriptHeader& header, ScriptBody& body) : header(header), body(body) {}

			ScriptHeader& header;
			ScriptBody& body;
			std::unordered_set<std::string> scripts{};
		};
	}
}
