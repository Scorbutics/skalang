#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "Interpreter/Value/ScriptHandle.h"
#include "NodeValue/ScriptCacheAST.h"
#include "Container/order_indexed_string_map.h"

namespace ska {
	class ScriptCache {
	public:
		ScriptCacheAST astCache;
		order_indexed_string_map<std::unique_ptr<ScriptHandle>> cache;
	};
}