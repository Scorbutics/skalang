#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "Interpreter/Value/ScriptHandle.h"
#include "NodeValue/ScriptCacheAST.h"

namespace ska {
	class ScriptCache {
	public:
		ScriptCacheAST astCache;
		ScriptCacheBase<std::unique_ptr<ScriptHandle>> cache;
	};
}