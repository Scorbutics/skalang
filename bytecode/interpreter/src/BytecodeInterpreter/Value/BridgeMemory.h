#pragma once
#include <memory>

namespace ska {
	namespace bytecode {
		struct BridgeFunction;
		using BridgeMemory = std::shared_ptr<BridgeFunction>;
	}
}