#pragma once
#include "NodeValueArray.h"

namespace ska {
	struct NodeValueFunctionMember {
		NodeValueArray owner = nullptr;
		NodeValue* functionMember = nullptr;
	};
}
