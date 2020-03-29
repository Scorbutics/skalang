#pragma once

#include "ScriptHandleAST.h"
#include "Base/Containers/order_indexed_string_map.h"

namespace ska {
	struct ScriptCacheAST :
		public order_indexed_string_map<std::unique_ptr<ScriptHandleAST>> {
	};
}
