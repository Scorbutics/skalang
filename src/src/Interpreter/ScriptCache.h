#pragma once
#include <unordered_map>
#include <string>
#include "Interpreter/Value/ScriptHandle.h"

namespace ska {
	using ScriptCache = std::unordered_map<std::string, ScriptHandlePtr>;
}