#pragma once
#include <variant>
#include <memory>

#include "BridgeMemory.h"
#include "NodeValue/ObjectMemory.h"
#include "NodeValue/StringShared.h"
#include "ScriptVariableRef.h"

namespace ska {
	using TokenVariant = std::variant<ScriptVariableRef, bool, long, double, StringShared, ObjectMemory, BridgeMemory>;
}
