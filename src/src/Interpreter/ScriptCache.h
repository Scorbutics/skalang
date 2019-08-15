#pragma once
#include <unordered_map>
#include <string>
#include "Interpreter/Value/ScriptHandle.h"
#include "NodeValue/ScriptCacheAST.h"

namespace ska {
	struct ScriptCache {
		ScriptCacheAST astCache;
		std::unordered_map<std::string, ScriptHandlePtr> cache;
	};
}