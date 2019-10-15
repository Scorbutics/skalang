#pragma once

#include "NodeValue/Type.h"
#include "Runtime/Value/NodeValue.h"

namespace ska {
	struct TypedNodeValue {
		NodeValue value;
		Type type;
	};
}
