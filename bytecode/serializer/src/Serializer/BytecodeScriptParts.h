#pragma once
#include <cstddef>

#include "BytecodeScriptHeader.h"
#include "BytecodeScriptBody.h"
#include "BytecodeScriptExternalReferences.h"

namespace ska {
	namespace bytecode {
		struct ScriptParts {
			ScriptParts() : body(natives), header(natives), references(natives) {}

			std::vector<std::string> natives;
			ScriptHeader header;
			ScriptBody body;
			ScriptExternalReferences references;
		};
	}
}
