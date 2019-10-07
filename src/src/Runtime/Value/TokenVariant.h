#pragma once
#include <variant>
#include <memory>

#include "BridgeMemory.h"
#include "ObjectMemory.h"
#include "StringShared.h"
#include "ScriptVariableRef.h"

namespace ska {
	using TokenVariant = std::variant<ScriptVariableRef, bool, long, double, StringShared, ObjectMemory, BridgeMemory>;
}
