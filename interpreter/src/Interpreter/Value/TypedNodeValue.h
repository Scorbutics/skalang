#pragma once

#include "NodeValue/Type.h"
#include "NodeValue.h"

namespace ska {
	struct TypedNodeValue {
		NodeValue value;
		Type type;
	};
}
