#pragma once
#include <variant>
#include <memory>

#include "BridgeMemory.h"
#include "NodeValue/ObjectMemory.h"
#include "NodeValue/StringShared.h"
#include "Interpreter/Value/ScriptVariableRef.h"

namespace ska {
	using TokenVariant = std::variant<ScriptVariableRef, bool, int, double, StringShared, ObjectMemory, BridgeMemory>;
}
