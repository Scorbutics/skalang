#pragma once
#include <string>
#include <unordered_map>
#include "ScriptHandleAST.h"

namespace ska {
	using ScriptCacheAST = std::unordered_map<std::string, ScriptHandleASTPtr>;
}
