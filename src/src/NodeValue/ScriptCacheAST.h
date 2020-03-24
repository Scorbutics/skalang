#pragma once

#include "ScriptHandleAST.h"
#include "Container/order_indexed_string_map.h"

namespace ska {
	struct ScriptCacheAST :
		public order_indexed_string_map<std::unique_ptr<ScriptHandleAST>> {
	};
}
