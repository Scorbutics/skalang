#pragma once
#include <memory>

namespace ska {
	struct BridgeFunction;
	using BridgeMemory = std::shared_ptr<BridgeFunction>;
}